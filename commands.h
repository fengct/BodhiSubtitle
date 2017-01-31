#ifndef COMMANDS_H
#define COMMANDS_H

#include "bodhicore.h"

class BodhiSession;
class Command
{
public:
    Command(BodhiSession &session);

    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual void redo() = 0;
    virtual bool timeModified() const { return false;}
    int getActiveLine() const { return m_activeLine; }

    enum CmdExecuteFlag{
        E_Unkown,
        E_NewTask,
        E_Undo,
        E_Redo
    };
    CmdExecuteFlag executeFlag() const { return m_execFlag; }
    void setExecuteFlag(CmdExecuteFlag flag) { m_execFlag = flag; }
protected:
    BodhiSession &m_session;
    CmdExecuteFlag m_execFlag;
    int m_activeLine;
};

class AddRecordCommand : public Command
{
public:
    AddRecordCommand(BodhiSession &session, qint64 pos, qint64 length);

    virtual void execute();
    virtual void undo();
    virtual void redo();
protected:
    SrtRecordPtr m_record;
    qint64 m_pos;
    qint64 m_length;
};

class DelRecordCommand : public Command
{
public:
    DelRecordCommand(BodhiSession &session, int seq);

    virtual void execute();
    virtual void undo();
    virtual void redo();
protected:
    SrtRecordPtr m_sentence;
};

class SplitCommand : public Command
{
public:
    SplitCommand(BodhiSession &session, int seq, unsigned long pos);

    virtual void execute();
    virtual void undo();
    virtual void redo();
protected:
    qint64 m_pos;
    SrtRecordPtr m_originSentence;
    SrtRecordPtr m_splitSentence1;
    SrtRecordPtr m_splitSentence2;
};

class MergeCommand : public Command
{
public:
    MergeCommand(BodhiSession &session, int seq1, int seq2);

    virtual void execute();
    virtual void undo();
    virtual void redo();
protected:
    SrtRecordPtr m_originSentence1;
    SrtRecordPtr m_originSentence2;
    SrtRecordPtr m_mergedSentence;
};

class ContentChange : public Command
{
public:
    ContentChange(BodhiSession &session, int seq, const QString &contentOld);

    virtual void execute();
    virtual void undo();
    virtual void redo();
private:
    int		 m_seq;
    QString	 m_content;
    bool	 m_proof;
};

#endif // COMMANDS_H
