#include "baseplaintextedit.h"

#define ZOOM    4

BasePlainTextEdit::BasePlainTextEdit(QWidget *parent): QPlainTextEdit(parent)
{
    deleteLineAction = new QAction(QIcon(":/res/images/erase.png"), tr("Delete Line"), this);
    deleteLineAction->setShortcut(Qt::CTRL|Qt::Key_Delete);
    connect(deleteLineAction, SIGNAL(triggered()), this, SLOT(deleteLineAtCursor()));

    smartSlashCommentAction = new QAction(QIcon(":/res/images/comment_slash.png"), tr("Smart Slash Comment"), this);
    smartSlashCommentAction->setShortcut(Qt::CTRL|Qt::Key_Slash);
    connect(smartSlashCommentAction, SIGNAL(triggered()), this, SLOT(smartSlashComment()));

    smartBlockCommentAction = new QAction(QIcon(":/res/images/comment_block.png"), tr("Smart Block Comment"), this);
    smartBlockCommentAction->setShortcut(Qt::CTRL|Qt::SHIFT|Qt::Key_Slash);
    connect(smartBlockCommentAction, SIGNAL(triggered()), this, SLOT(smartBlockComment()));

    undoAction = new QAction(QIcon(":/res/images/undo.png"), tr("Undo"), this);
    undoAction->setShortcut(QKeySequence::Undo);
    connect(undoAction, SIGNAL(triggered()), this, SLOT(undo()));

    redoAction = new QAction(QIcon(":/res/images/redo.png"), tr("Redo"), this);
    redoAction->setShortcut(QKeySequence::Redo);
    connect(redoAction, SIGNAL(triggered()), this, SLOT(redo()));

    cutAction = new QAction(QIcon(":/res/images/cut.png"), tr("Cut"), this);
    cutAction->setShortcut(QKeySequence::Cut);
    connect(cutAction, SIGNAL(triggered()), this, SLOT(cut()));

    copyAction = new QAction(QIcon(":/res/images/copy.png"), tr("Copy"), this);
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAction = new QAction(QIcon(":/res/images/paste.png"), tr("Paste"), this);
    pasteAction->setShortcut(QKeySequence::Paste);
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(paste()));

    deleteAction = new QAction(QIcon(":/res/images/erase.png"), tr("Delete"), this);
    deleteAction->setShortcut(QKeySequence::Delete);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteSelected()));

    selectAllAction = new QAction(QIcon(":/res/images/selectall.png"), tr("Select All"), this);
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    connect(selectAllAction, SIGNAL(triggered()), this, SLOT(selectAll()));

    lowercaseAction = new QAction(QIcon(":/res/images/lowercase.png"), tr("Lowercase"), this);
    lowercaseAction->setShortcut(Qt::CTRL|Qt::Key_L);
    connect(lowercaseAction, SIGNAL(triggered()), this, SLOT(lowercaseSelectedCode()));

    uppercaseAction = new QAction(QIcon(":/res/images/uppercase.png"), tr("Uppercase"), this);
    uppercaseAction->setShortcut(Qt::CTRL|Qt::Key_U);
    connect(uppercaseAction, SIGNAL(triggered()), this, SLOT(uppercaseSelectedCode()));

    undoAction->setEnabled(false);
    redoAction->setEnabled(false);
    connect(this, SIGNAL(undoAvailable(bool)), undoAction, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(redoAvailable(bool)), redoAction, SLOT(setEnabled(bool)));

    toogleBookmark = new QAction(QIcon(":/res/images/bookmark.png"), "Toggle Bookmark", this);
    toogleBookmark->setShortcut(Qt::CTRL|Qt::Key_B);

    resetZoom();
}

BasePlainTextEdit::~BasePlainTextEdit()
{
    delete smartSlashCommentAction;
    delete deleteLineAction;
    delete undoAction;
    delete redoAction;
    delete cutAction;
    delete copyAction;
    delete pasteAction;
    delete deleteAction;
    delete selectAllAction;
    delete uppercaseAction;
    delete lowercaseAction;
    delete toogleBookmark;
}

QMenu *BasePlainTextEdit::createMenu()
{
    QMenu *menu = new QMenu;
    QTextCursor textCursor = this->textCursor();

    //! if nothing selected
    if (textCursor.selectionEnd() - textCursor.selectionStart() <= 0)
    {
        uppercaseAction->setEnabled(false);
        lowercaseAction->setEnabled(false);
        cutAction->setEnabled(false);
        copyAction->setEnabled(false);
        deleteAction->setEnabled(false);
    }
    else
    {
        uppercaseAction->setEnabled(true);
        lowercaseAction->setEnabled(true);
        cutAction->setEnabled(true);
        copyAction->setEnabled(true);
        deleteAction->setEnabled(true);
    }

    undoAction->setVisible(!isReadOnly());
    redoAction->setVisible(!isReadOnly());
    cutAction->setVisible(!isReadOnly());
    pasteAction->setVisible(!isReadOnly());
    deleteAction->setVisible(!isReadOnly());

    menu->addAction(deleteLineAction);
    menu->addSeparator();
    menu->addAction(smartSlashCommentAction);
    menu->addAction(smartBlockCommentAction);
    menu->addSeparator();
    menu->addAction(undoAction);
    menu->addAction(redoAction);
    menu->addSeparator();
    menu->addAction(cutAction);
    menu->addAction(copyAction);
    menu->addAction(pasteAction);
    menu->addAction(deleteAction);
    menu->addSeparator();
    menu->addAction(selectAllAction);
    menu->addSeparator();
    menu->addAction(uppercaseAction);
    menu->addAction(lowercaseAction);
    menu->addSeparator();
    menu->addAction(toogleBookmark);
    return menu;
}

int BasePlainTextEdit::getZoom()
{
    return zoom;
}

void BasePlainTextEdit::resetZoom()
{
    zoom = 0;
}

void BasePlainTextEdit::contextMenuEvent(QContextMenuEvent *e)
{
    QMouseEvent leftClick(QEvent::MouseButtonPress, e->pos(), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    mousePressEvent(&leftClick);
    contextMenu = createMenu();
    contextMenu->exec(e->globalPos());
    if (!contextMenu.isNull())
        delete contextMenu;
}

void BasePlainTextEdit::wheelEvent(QWheelEvent *e)
{
    if( e->modifiers() == Qt::ControlModifier )
    {
        QPoint aDelta = e->angleDelta();
        if(aDelta.y() > 0)
        {

            if(zoom < ZOOM){
                zoomIn();
                zoom++;
            }
        }
        else
        {

            if(zoom > -ZOOM){
                zoomOut();
                zoom--;
            }
        }
    }
    QPlainTextEdit::wheelEvent(e);
}

// qDebug("test=%s", qPrintable("\\n"));

/**
 * @brief
 * On commentMode being true the shadowCursor's current block gets given insertMarker inserted
 * while on commentMode being false the given removeRegExp gets applied and
 * @param commentMode  defines the action (true = add comment, false = remove comment)
 * @param shadowCursor text cursor where the cursor defines the line to apply the action to
 * @param removeRegExp uncomment: regular expression that defines the group of text to remove
 * @param insertMarker comment: marker to insert at the begin of the current cursor line
 * @return number of characters the selection added/removed during the changes
 */
int changeCommentMarker(bool commentMode, QTextCursor shadowCursor, QRegExp removeRegExp, QString insertMarker)
{
    shadowCursor.movePosition(QTextCursor::StartOfLine);
    shadowCursor.select(QTextCursor::BlockUnderCursor);
    QString shadowSelected = shadowCursor.selectedText();
    int lengthBeforeChange = shadowSelected.size();
    if (commentMode)
    {
        // due to BlockUnderCursor selection the char 0 is the linefeed
        shadowSelected.insert(1, insertMarker);
    }
    else
    {
        // due to BlockUnderCursor selection the char 0 is the linefeed
        int index = removeRegExp.indexIn(shadowSelected);
        if (index > -1)
        {
            shadowSelected.replace(index+removeRegExp.pos(1), removeRegExp.cap(1).size(), "");
        }
    }
    shadowCursor.insertText(shadowSelected);
    return (shadowSelected.size() - lengthBeforeChange);
}

int resolveLineCount(QTextCursor tc)
{
    // determine how many lines are to affect
    if (!tc.hasSelection())
    {
        tc.select(QTextCursor::LineUnderCursor);
    }
    QString selected = tc.selectedText();
    int numberOfLines = (selected.isEmpty() ? 1 : selected.count("\u2029")+1);
    return numberOfLines;
}

/**
 * @brief
 * Identifies what action has to be taken (add comment or remove comment)
 * @param tc                text cursor to operate with
 * @param cursorMovement    movement to apply before the check
 * @param marker            marker to detect
 * @return true indicates add comment, false remove comment action
 */
bool identifyAction(QTextCursor tc, QTextCursor::MoveOperation cursorMovement, QRegExp marker)
{
    QTextCursor tc2 = QTextCursor(tc);
    if (cursorMovement != QTextCursor::NoMove)
    {
        tc2.movePosition(cursorMovement);
    }

    tc2.select(QTextCursor::BlockUnderCursor);
    QString selected2 = tc2.selectedText();
    // check if the marker exists
    int index = selected2.indexOf(marker);
    bool addCommentAction = (index == -1);
    return addCommentAction;
}

QTextCursor applySelection(QTextCursor tc, bool bottomUpDirection, int cursorPosition, int anchorPosition)
{
    if (bottomUpDirection)
    {
        tc.setPosition(anchorPosition);
        tc.setPosition(cursorPosition, QTextCursor::KeepAnchor);
    }
    else
    {
        tc.setPosition(cursorPosition);
        tc.setPosition(anchorPosition, QTextCursor::KeepAnchor);
    }
    return tc;
}

void BasePlainTextEdit::smartSlashComment()
{
    QRegExp markerAtLineStartWithIndent = QRegExp("^[\n\u2029]{1}([\\t ]*//[ ]?)");
    QTextCursor tc = textCursor();

    // determine how many lines are to affect
    int numberOfLines = resolveLineCount(tc);
    bool cursorAtLineStart = numberOfLines>1 && tc.atBlockStart();

    // determine direction of selection
    int cursorPosition = tc.position();
    int anchorPosition = tc.anchor();
    bool bottomUpDirection = (anchorPosition < cursorPosition);

    // determine the action to apply
    // if the cursor is at the block start that line is not part of the action
    QTextCursor::MoveOperation moveDirection = (bottomUpDirection ? QTextCursor::Up : QTextCursor::Down);
    QTextCursor::MoveOperation cursorMovement = (cursorAtLineStart ? moveDirection : QTextCursor::NoMove);
    bool addCommentAction = identifyAction(tc, cursorMovement, markerAtLineStartWithIndent);

    // add/remove comment for each line
    QTextCursor shadowCursor = QTextCursor(tc);

    if (cursorAtLineStart)
    {
        // ignore line where the cursor stays
        shadowCursor.movePosition(bottomUpDirection ? QTextCursor::Up : QTextCursor::Down);
        numberOfLines--;
    }

    int totalChanges = 0;
    for (int i = 0; i < numberOfLines; ++i)
    {
        int changes = changeCommentMarker(
                    addCommentAction, shadowCursor, markerAtLineStartWithIndent, QString("// "));
        totalChanges += changes;
        shadowCursor.movePosition(bottomUpDirection ? QTextCursor::Up : QTextCursor::Down);
    }

    // a single line operation?
    if (numberOfLines == 1)
    {
        // yes, place cursor to the start of the next block to allow line-wise block operation
        tc.clearSelection();
        tc.movePosition(QTextCursor::NextBlock);
        tc.movePosition(QTextCursor::StartOfLine);
        setTextCursor(tc);
    }
    else
    {
        // restore the selection with the applied changes
        int factor = addCommentAction ? +3 : -3;
        int newAnchorPos = anchorPosition + (bottomUpDirection ? factor : totalChanges);
        int newCursorPos = cursorPosition + (bottomUpDirection ? totalChanges : factor);
        tc = applySelection(tc, bottomUpDirection, newCursorPos, newAnchorPos);
        setTextCursor(tc);
    }
}

void BasePlainTextEdit::smartBlockComment()
{
    QRegExp openBlockRegExp = QRegExp("^([\n\u2029]{1}[\\t ]*/\\*)");
    QRegExp closeBlockRegExp = QRegExp("^([\n\u2029]{1}[\\t ]*\\*/)");

    QTextCursor tc = textCursor();

    // determine direction of selection
    int cursorPosition = tc.position();
    int anchorPosition = tc.anchor();
    bool bottomUpDirection = (anchorPosition <= cursorPosition);

    // determine how many lines are to affect
    int numberOfLines = resolveLineCount(tc);

    // relocate cursor at the line start
    if (numberOfLines>1 && tc.atBlockStart())
    {
        // relocate cursor at line start without selection
        tc.movePosition(bottomUpDirection ? QTextCursor::Up : QTextCursor::Down);
        numberOfLines--;
    }

    // determine the action to apply
    QRegExp marker = bottomUpDirection ? closeBlockRegExp : openBlockRegExp;
    QTextCursor::MoveOperation moveDirection =
            (bottomUpDirection ? QTextCursor::NextBlock : QTextCursor::PreviousBlock);
    bool addCommentAction = identifyAction(tc, moveDirection, marker);

    // adjust cursor position on bottom-up
    if (bottomUpDirection)
    {
        // start on the next line
        tc.movePosition(QTextCursor::NextBlock);
        // and on adding comments move an additional line upward
        if (addCommentAction)
        {
            numberOfLines++;
        }
    }

    QTextCursor shadowCursor = QTextCursor(tc);
    // add/remove block comment
    QRegExp startRegexp = bottomUpDirection ? closeBlockRegExp : openBlockRegExp;
    QString startMarker = QString(bottomUpDirection ? "*/\n" : "/*\n");
    changeCommentMarker(addCommentAction, shadowCursor, startRegexp, startMarker);

    // move cursor n lines to the place of the next marker
    for (int i = 0; i < numberOfLines; ++i)
    {
        shadowCursor.movePosition(bottomUpDirection ? QTextCursor::Up : QTextCursor::Down);
    }

    // add/remove block comment
    QRegExp endRegexp = bottomUpDirection ? openBlockRegExp : closeBlockRegExp;
    QString endMarker = QString(bottomUpDirection ? "/*\n" : "*/\n");
    changeCommentMarker(addCommentAction, shadowCursor, endRegexp, endMarker);

    // restore the selection with the applied changes
    int factor = addCommentAction ? +3 : -3;
    int newAnchorPos = anchorPosition + factor;
    int newCursorPos = cursorPosition + factor;
    tc = applySelection(tc, bottomUpDirection, newCursorPos, newAnchorPos);
    setTextCursor(tc);
}

void BasePlainTextEdit::deleteLineAtCursor()
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::BlockUnderCursor);
    if(tc.hasSelection())
    {
        tc.removeSelectedText();
    }

}

void BasePlainTextEdit::deleteSelected()
{
    textCursor().removeSelectedText();
}

void BasePlainTextEdit::uppercaseSelectedCode()
{
    textCursor().insertText(textCursor().selectedText().toUpper());
}

void BasePlainTextEdit::lowercaseSelectedCode()
{
    textCursor().insertText(textCursor().selectedText().toLower());
}
