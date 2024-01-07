#include "processes.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <stdio.h>
#include <messages.h>
#include <file-properties.h>
#include <sync.h>
#include <string.h>
#include <errno.h>

/*!
 * @brief prepare prepares (only when parallel is enabled) the processes used for the synchronization.
 * @param the_config is a pointer to the program configuration
 * @param p_context is a pointer to the program processes context
 * @return 0 if all went good, -1 else
 */
int prepare(configuration_t *the_config, process_context_t *p_context) {
    // Initialize p_context based on the_config
    // Return 0 if successful, -1 otherwise


    if (the_config->is_parallel) {
        p_context->message_queue_id = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

        if (p_context->message_queue_id == -1) {
            perror("Erreur lors de la création de la file de messages");
            return -1;
        }


        p_context->destination_analyzers_pids = make_process(p_context, analyzer_process_loop, NULL);
        if (p_context->destination_analyzers_pids == -1) {
            perror("Erreur lors de la création du processus d'analyse");
            return -1;
        }


        p_context->destination_lister_pid = make_process(p_context, lister_process_loop, NULL);
        if (p_context->destination_lister_pid == -1) {
            perror("Erreur lors de la création du processus de listage");
            return -1;
        }
    }

    return 0;

}

/*!
 * @brief make_process creates a process and returns its PID to the parent
 * @param p_context is a pointer to the processes context
 * @param func is the function executed by the new process
 * @param parameters is a pointer to the parameters of func
 * @return the PID of the child process (it never returns in the child process)
 */
int make_process(process_context_t *p_context, process_loop_t func, void *parameters) {
    // Fork a new process
    // In the child process, call func(parameters)
    // In the parent process, return the child's PID
    
    pid_t pid = fork();
    if (pid == -1) {
        perror("Erreur lors de la création du processus");
        return -1;
    }
    if (pid == 0) {
        func(parameters);
        exit(EXIT_SUCCESS);
    }
    return pid;
}

/*!
 * @brief lister_process_loop is the lister process function (@see make_process)
 * @param parameters is a pointer to its parameters, to be cast to a lister_configuration_t
*/

void lister_process_loop(void *parameters) {
    // Cast parameters to lister_configuration_t
    // List files based on the configuration
    lister_configuration_t *cfg = (lister_configuration_t *) parameters;
    analyze_dir_command_t message; 
    simple_command_t message_end; 
    int msg_q_id = msgget(cfg->mq_key, 0666);
    bool loop = true;
    while(loop){
        if(msgrcv(msg_q_id, &message, sizeof(message.op_code), cfg->my_receiver_id, 0) != -1){
            loop = false;
        }
    }
        if (strcmp(message.op_code, COMMAND_CODE_ANALYZE_DIR) == 0) {
            //The process is asked to make a list out of this directory
            //Build the list 
            files_list_t l; 
            l.head = NULL; 
            l.tail = NULL; 
            make_list(&l, message.target); 

            //Send the n-th first element of the list to the n analyzers 
            files_list_entry_t *p = l.head;
            int sent_requests = cfg->analyzers_count;
            int answer_received = 0; 
            analyze_file_command_t response; 
            while(p != NULL || answer_received != sent_requests){
                if (msgrcv(msg_q_id, &response, sizeof(response.op_code), cfg->my_receiver_id, 0) != -1) {
                    //The lister got an answer from one of the n analyzers 
                    if(strcmp(response.op_code, COMMAND_CODE_FILE_ANALYZED) == 0){
                        //The analyzer finished its work
                        send_files_list_element(msg_q_id, cfg->my_receiver_id, &response.payload);
                        answer_received++; 
                        sent_requests--; 
                    }

                    //If one of the analyzer is unoccupied, send it another element of the list
                    if (sent_requests != cfg->analyzers_count && p != NULL) {
                        send_analyze_file_command(msg_q_id, cfg->my_recipient_id, p);
                        p = p->next; 
                        answer_received--;
                    }
                }
            }
            //Send the freshly received datas to the main
            if (cfg->my_receiver_id == MSG_TYPE_TO_MAIN_FROM_DESTINATION_LISTER){
                send_list_end(msg_q_id, MSG_TYPE_TO_MAIN_FROM_END_DEST_LISTER); 
            }
            else if (cfg->my_receiver_id == MSG_TYPE_TO_MAIN_FROM_SOURCE_LISTER){
                send_list_end(msg_q_id, MSG_TYPE_TO_MAIN_FROM_END_SRC_LISTER); 
            }
        } 
    
    if (msgrcv(msg_q_id, &message_end, sizeof(message_end.message), cfg->my_receiver_id, 0) != -1) {
        if (strcmp(message_end.message, COMMAND_CODE_TERMINATE) == 0) {
            send_terminate_confirm(msg_q_id, cfg->my_recipient_id);
        }
    }
}



/*!
 * @brief analyzer_process_loop is the analyzer process function
 * @param parameters is a pointer to its parameters, to be cast to an analyzer_configuration_t
 */
void analyzer_process_loop(void *parameters) {
    // Cast parameters to analyzer_configuration_t
    // Analyze files based on the configuration
    lister_configuration_t *cfg = (lister_configuration_t *)parameters;
    
    simple_command_t msg_end;
    analyze_file_command_t msg;
    
    
    int msg_id = msgget(cfg->mq_key, 0666);
    bool loop = true;
    
    
    while (loop) {

        if (msgrcv(msg_id, &msg, sizeof(msg) - sizeof(long), cfg->my_receiver_id, 0) != -1) {
            get_file_stats(&msg.payload);
            send_analyze_file_response(msg_id, cfg->my_recipient_id, &msg.payload);
        }


        //When a terminaison message is received, it stops the process
        if (msgrcv(msg_id, &msg_end, sizeof(msg_end.message), MSG_TYPE_TO_SOURCE_ANALYZERS, 0) != -1) {
            if (strcmp(msg_end.message, COMMAND_CODE_TERMINATE) == 0) {
                loop = false;
                send_terminate_confirm(msg_id, cfg->my_recipient_id); 
            }
        }


    }
}

/*!
 * @brief clean_processes cleans the processes by sending them a terminate command and waiting to the confirmation
 * @param the_config is a pointer to the program configuration
 * @param p_context is a pointer to the processes context
 */
void clean_processes(configuration_t *the_config, process_context_t *p_context) {
    // Send a terminate command to all processes in p_context
    // Wait for responses from all processes
    // Free any memory allocated in p_context
    // Free the message queue



   if (the_config->is_parallel) {
        int loop_dl = 1;
        int loop_sl = 1;
        int loop_da = 1;
        int loop_sa = 1;
        simple_command_t end_sig;


        send_terminate_command(p_context->message_queue_id, MSG_TYPE_TO_DESTINATION_LISTER); 
        send_terminate_command(p_context->message_queue_id, MSG_TYPE_TO_SOURCE_LISTER);
        send_terminate_command(p_context->message_queue_id, MSG_TYPE_TO_DESTINATION_ANALYZERS);
        send_terminate_command(p_context->message_queue_id, MSG_TYPE_TO_SOURCE_ANALYZERS);


        while (loop_dl && loop_sl && loop_sa && loop_da) { 
            if (msgrcv(p_context->message_queue_id, &end_sig, sizeof(end_sig.message), MSG_TYPE_TO_DESTINATION_LISTER, 0) != -1){ 
                loop_dl = 0;
            }
            if (msgrcv(p_context->message_queue_id, &end_sig, sizeof(end_sig.message), MSG_TYPE_TO_SOURCE_LISTER, 0) != -1){ 
                loop_sl = 0;
            }
            if (msgrcv(p_context->message_queue_id, &end_sig, sizeof(end_sig.message), MSG_TYPE_TO_DESTINATION_ANALYZERS, 0) != -1){ 
                loop_da = 0;
            }
            if (msgrcv(p_context->message_queue_id, &end_sig, sizeof(end_sig.message), MSG_TYPE_TO_SOURCE_ANALYZERS, 0) != -1){ 
                loop_sa = 0;
            }
        }


        free(p_context->source_analyzers_pids);
        free(p_context->destination_analyzers_pids);


        msgctl(p_context->message_queue_id, IPC_RMID, NULL);
    }
}