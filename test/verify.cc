#include <gmi_mesh.h>
#include <apfMDS.h>
#include <apfMesh2.h>
#include <apf.h>
#include <PCU.h>

char** argv;

void* thread_main(void*)
{
  apf::Mesh2* m = apf::loadMdsMesh(argv[1],argv[2]);
  m->verify();
  m->destroyNative();
  apf::destroyMesh(m);
  return NULL;
}

int main(int argc, char** argv_in)
{
  assert(argc==4);
  argv = argv_in;
  int provided;
  MPI_Init_thread(&argc,&argv,MPI_THREAD_MULTIPLE,&provided);
  assert(provided == MPI_THREAD_MULTIPLE);
  PCU_Comm_Init();
  gmi_register_mesh();
  PCU_Thrd_Run(atoi(argv[3]), thread_main, NULL);
  PCU_Comm_Free();
  MPI_Finalize();
}

