#include <PCU.h>
#include <assert.h>

void* thread_main(void* arg)
{
  fprintf(stderr,"hello!\n");
  double data[2] = {1.0, 2.0};
  PCU_Add_Doubles(data, 2);
  fprintf(stderr,"%d done! (%f %f)\n", PCU_Comm_Self(), data[0], data[1]);
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
