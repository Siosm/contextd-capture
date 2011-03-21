#ifndef TYPEDDELEGATE_H
#define TYPEDDELEGATE_H

#include <QItemDelegate>

/// A delegate for editing data with multiple possible types, using a TypedLineEdit, QSpinBox, QCheckBox and QComboBox
class TypedDelegate : public QItemDelegate
{
    Q_OBJECT
public:
	/*!
	 * \brief Construct a Typed Delegate with data integrity checking
	 * \param type The type of the delegate (see VariableType)
	 * \param column The only column on which editing must be allowed
	 * \param parent The parent QObject for this object
	 * \param data1 Type-dependant user data (String/Regexp: validation expression; List: list of items; Int: min bound)
	 * \param data2 Type-dependant user data (String/Regexp: blacklist; Int: max bound)
	 */
	explicit TypedDelegate(int type, int column, QObject *parent=0, QVariant data1=QVariant(), QVariant data2=QVariant());

	/// Return the widget used to edit the item specified by index for editing
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	/// Set the data to be displayed and edited by the editor from the data model item specified by the model index
	void setEditorData(QWidget *editor, const QModelIndex &index) const;

	/// Get data from the editor widget and stores it in the specified model at the item index
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

	/// Update the editor for the item specified by index according to the style option given
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
	int _type;
	int _column;
	QVariant _data1;
	QVariant _data2;
};

#endif // TYPEDDELEGATE_H
