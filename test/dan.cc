#include <PCU.h>
#include <assert.h>

void* thread_main(void* arg)
{
  fprintf(stderr,"hello!\n");
  PCU_Barrier();
  fprintf(stderr,"%d done!\n", PCU_Comm_Self());
  return NULL;
}

int main(int argc, char** argv)
{
  int provided;
  MPI_Init_thread(&argc,&argv,MPI_THREAD_MULTIPLE,&provided);
  assert(provided==MPI_THREAD_MULTIPLE);
  PCU_Comm_Init();
  PCU_Protect();
  PCU_Thrd_Run(2, thread_main, NULL);
  fprintf(stderr,"ok!\n");
  PCU_Comm_Free();
  MPI_Finalize();
}
