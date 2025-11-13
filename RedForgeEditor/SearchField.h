#pragma once

#include <QComboBox>
#include <QLineEdit>
#include <QStringListModel>
#include <QListView>
#include <QKeyEvent>
#include <QApplication>
#include <QEvent>
#include <QTimer>

class SearchField : public QComboBox
{
    Q_OBJECT

private:
    QStringList stringList;
    QStringListModel* stringListModel = nullptr;
    QListView* dropdownList = nullptr;

    int selectedIndex = -1;

public:
    explicit SearchField(const QString& placeholderText, QWidget* parent = nullptr);
    ~SearchField();

    int GetSelectedIndex() { return selectedIndex; };

    void SetItems(const QStringList& items);
    void SetSelectedIndex(int index);

protected:
    void showPopup() override;
    void hidePopup() override;

    void mousePressEvent(QMouseEvent* event) override;

    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private:
    void OnTextEdited(const QString& text);
    void OnItemSelected(const QModelIndex& modelIndex);

signals:
    void OnSelectionChanged(int index);
};