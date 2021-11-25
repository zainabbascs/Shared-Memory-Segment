#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#define BUFFER_SIZE 10
typedef struct{
int pc_buffer[BUFFER_SIZE];
int in;
int out;
} my_buffer;
int main(){
/* the identifier for the shared memory segment */
int segment_id;
/* a pointer to the shared memory segment */
my_buffer *shared_memory1, *shared_memory2;
/* the size (in bytes) of the shared memory segment */
int segment_size, i;
pid_t pid;
segment_size = sizeof(my_buffer);

** allocate a shared memory segment */
segment_id = shmget(IPC_PRIVATE, segment_size, S_IRUSR | 
S_IWUSR); 
/* IPC_PRIVATE = create new IPC
S_IRUSR, S_IWUSR = permission user readable, writeable 
*/
/** attach the shared memory segment */
shared_memory1 = (my_buffer *) shmat(segment_id, NULL, 0);
printf("shared memory segment %d attached at address %p\n", 
segment_id, shared_memory1);
/* Initialize in and out values */
shared_memory1 -> in = 0;
shared_memory1 -> out = 0;
pid = fork();

if (pid == 0) { /* child process is producer */
/** attach the shared memory segment */
shared_memory2 = (my_buffer *) shmat(segment_id, NULL, 0);
for (i = 0; i < 20; i++) {
while ((shared_memory2->in + 1) % BUFFER_SIZE ==
shared_memory2->out) { /* Do Nothing */ }
/* Add item to buffer */
shared_memory2->pc_buffer[shared_memory2->in] = i;
/* Update in pointer. */
shared_memory2->in = (shared_memory2->in+1)%BUFFER_SIZE;
} /* End for; produces 20 items */
/** detach the shared memory segment */
shmdt((void*)shared_memory2);
exit(0);
}

else { /* parent process is consumer */
for (i = 0; i < 20; i++) {
while (shared_memory1->in == shared_memory1->out){
 /* Busy polling. Do nothing useful. */ }
/* Update out pointer. */
shared_memory1->out = (shared_memory1->out+1)%BUFFER_SIZE;
} /* End for; consumes 20 items */
/** detach the shared memory segment */
shmdt((void*)shared_memory1);
wait (NULL);
/** remove the shared memory segment */
shmctl(segment_id, IPC_RMID, NULL);
exit (0);
}
return 0;
}