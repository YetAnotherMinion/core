#include <PCU.h>
#include <assert.h>

void* thread_main(void* arg)
{
  fprintf(stderr,"hello!\n");
  int data = 0;
  PCU_Comm_Begin();
  if (PCU_Comm_Self() > 1) {
    data = 42;
    PCU_COMM_PACK(1, data);
  }
  PCU_Comm_Send();
  while (PCU_Comm_Listen()) {
    PCU_COMM_UNPACK(data);
    fprintf(stderr,"%d got %d from %d\n",
        PCU_Comm_Self(), data, PCU_Comm_Sender());
  }
  double moredat[2] = {1, 7};
  PCU_Add_Doubles(moredat, 2);
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
  PCU_Thrd_Run(4, thread_main, NULL);
  fprintf(stderr,"ok!\n");
  PCU_Comm_Free();
  MPI_Finalize();
}
