#ifndef COMMANDS_H
#define COMMANDS_H

#include "bodhicore.h"

class Command
{
public:
    Command(BodhiSession &session);

    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual void redo() = 0;
    virtual bool isTimeModify() const { return FALSE;}
    int getActiveLine() const { return m_activeLine; }
protected:
    BodhiSession &m_session;
    int m_activeLine;
};

class AddRecordCommand : public Command
{
public:
    AddRecordCommand(BodhiSession &sessionBodhiSession &session, unsigned long pos, unsigned long length);

    virtual void execute();
    virtual void undo();
    virtual void redo();
protected:
    Sentence m_sentence;
    unsigned long m_pos;
    unsigned long m_length;
};

class DelRecordCommand : public Command
{
public:
    DelRecordCommand(CZimuView &view, CZimuFile &data, int seq);

    virtual void execute();
    virtual void undo();
    virtual void redo();
protected:
    Sentence *m_sentence;
};

class SplitCommand : public Command
{
public:
    SplitCommand(CZimuView &view, CZimuFile &data, int seq, unsigned long pos);

    virtual void execute();
    virtual void undo();
    virtual void redo();
protected:
    unsigned long m_pos;
    Sentence m_originSentence;
    Sentence m_splitSentence1;
    Sentence m_splitSentence2;
};

class MergeCommand : public Command
{
public:
    MergeCommand(CZimuView &view, CZimuFile &data, int seq1, int seq2);

    virtual void execute();
    virtual void undo();
    virtual void redo();
protected:
    Sentence m_originSentence1;
    Sentence m_originSentence2;
    Sentence m_mergedSentence;
};

class ContentChange : public Command
{
public:
    ContentChange(CZimuView &view, CZimuFile &data, int seq, const TString &contentOld);

    virtual void execute();
    virtual void undo();
    virtual void redo();
private:
    int		 m_seq;
    TString	 m_content;
    BOOL	 m_proof;
};

#endif // COMMANDS_H
