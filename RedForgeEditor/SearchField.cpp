#include "SearchField.h"

SearchField::SearchField(const QString& placeholderText, QWidget* parent) : QComboBox(parent)
{
    setEditable(true);
    setInsertPolicy(QComboBox::NoInsert);
    setCompleter(nullptr);

    lineEdit()->setPlaceholderText(placeholderText);

    stringListModel = new QStringListModel(this);

    dropdownList = new QListView(this);
    dropdownList->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    dropdownList->setAttribute(Qt::WA_ShowWithoutActivating);
    dropdownList->setFocusPolicy(Qt::ClickFocus);
    dropdownList->setModel(stringListModel);
    dropdownList->hide();

    connect(lineEdit(), &QLineEdit::textEdited, this, &SearchField::OnTextEdited);
    connect(dropdownList, &QListView::clicked, this, &SearchField::OnItemSelected);
}
SearchField::~SearchField()
{

}

void SearchField::SetItems(const QStringList& items)
{
    stringList = items;
    stringListModel->setStringList(items);
}
void SearchField::SetSelectedIndex(int index)
{
    selectedIndex = index;

    setCurrentIndex(index);
    setCurrentText((index >= 0 && index < stringList.size()) ? stringList[index] : "");
}

void SearchField::showPopup()
{
    OnTextEdited(lineEdit()->text());
}
void SearchField::hidePopup()
{
    dropdownList->hide();
}

void SearchField::mousePressEvent(QMouseEvent* event)
{
    clearEditText();

    QComboBox::mousePressEvent(event);
}

void SearchField::focusInEvent(QFocusEvent* event)
{
    showPopup();

    // Highlight the full text field upon initially clicking into it
    if(event->reason() == Qt::MouseFocusReason)
        QTimer::singleShot(0, this, 
            [this]()
            {
                lineEdit()->selectAll();
            });

    QComboBox::focusInEvent(event);
}
void SearchField::focusOutEvent(QFocusEvent* event)
{
    // Hide list when unfocused
    QTimer::singleShot(0, this, 
        [this]()
        {
            QWidget* next = QApplication::focusWidget();
            // If newly focused widget isn't the list, hide the list
            if(!next || (next != dropdownList && !dropdownList->isAncestorOf(next)))
                dropdownList->hide();

            // Refresh text field so it reflects selected item
            SetSelectedIndex(selectedIndex);
        });

    QComboBox::focusOutEvent(event);
}

void SearchField::OnTextEdited(const QString& text)
{
    QStringList filtered;
    // Filter items
    for(const QString& item : stringList)
        if(item.contains(text, Qt::CaseInsensitive))
            filtered << item;

    if(filtered.isEmpty())
    {
        dropdownList->hide();
        return;
    }

    stringListModel->setStringList(filtered);

    const int rowHeight = qMax(1, dropdownList->sizeHintForRow(0));
    const int visibleCount = qMin(filtered.size(), 8);
    const int height = rowHeight * visibleCount + 2 * dropdownList->frameWidth();

    const QPoint below = mapToGlobal(rect().bottomLeft());
    const QPoint above = mapToGlobal(rect().topLeft()) - QPoint(0, height);

    QRect screenRect = QGuiApplication::primaryScreen()->availableGeometry();
    bool fitsBelow = (below.y() + height <= screenRect.bottom());

    QPoint position = fitsBelow ? below : above;

    // Update geometry based on the visible items
    dropdownList->setGeometry(position.x(), position.y(), rect().width(), height);

    // Ensure it's on top of other windows, but doesn't activate
    dropdownList->raise();
    dropdownList->show();
}
void SearchField::OnItemSelected(const QModelIndex& modelIndex)
{
    const QString text = modelIndex.data().toString();
    // Find matching index in the full model
    int index = stringList.indexOf(text);
        
    QTimer::singleShot(0, this, [this, index]()
        {
            SetSelectedIndex(index);

            lineEdit()->clearFocus();

            emit OnSelectionChanged(index);
        });

    dropdownList->hide();
}