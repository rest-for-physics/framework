
#include <TRestRun.h>


int main()
{

    TRestRun *run = new TRestRun();

    run->OpenOutputFile();

    run->PrintStartDate();
    sleep(7);
    run->PrintEndDate();


    run->CloseOutputFile();

    run->GetRunLength();



    delete run;


}
