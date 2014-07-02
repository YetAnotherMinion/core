#include <PCU.h>
#include <assert.h>

void* thread_main(void* arg)
{
  double t0 = MPI_Wtime();
  double moredat[2] = {1,7};
  for (int i = 0; i < 10000; ++i)
    PCU_Add_Doubles(moredat, 2);
  double t1 = MPI_Wtime();
  if (!PCU_Comm_Self())
    fprintf(stderr,"%f seconds\n", t1 - t0);
  return NULL;
}

int main(int argc, char** argv)
{
  int provided;
  MPI_Init_thread(&argc,&argv,MPI_THREAD_MULTIPLE,&provided);
  assert(provided==MPI_THREAD_MULTIPLE);
  PCU_Comm_Init();
  PCU_Protect();
  PCU_Thrd_Run(16, thread_main, NULL);
  fprintf(stderr,"ok!\n");
  PCU_Comm_Free();
  MPI_Finalize();
}
