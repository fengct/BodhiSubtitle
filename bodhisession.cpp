#include "bodhisession.h"

BodhiSession::BodhiSession(QObject *parent) : QObject(parent)
{

}

bool BodhiSession::start(WorkType type, Work *workData)
{
    if (type == WT_DICATION){
        if (!workData){
            ; //start new dictation
            // open an exist sound file and create a subtitle file.
            // update history
        } else {
            // open two files
            // check if file exist, else propt user input and update work record.
        }
    } else if (type == WT_COLLATION) {
        if (!workData){
            ; //open subtitle file and sound file
            // updat history
        } else {
            // open two files
            // check if file exist, else propt user input and update work record.
        }
    }
}
