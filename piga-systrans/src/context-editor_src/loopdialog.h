#ifndef LOOPDIALOG_H
#define LOOPDIALOG_H

#include "abstractruledialog.h"
#include "loop.h"

/// A dialog for creating and editing Loop objects
class LoopDialog : public AbstractRuleDialog
{
	Q_OBJECT
public:
	/*!
	 * \brief Construct a New Loop Dialog
	 * \param parent The parent widget of the dialog
	 */
	explicit LoopDialog(QWidget *parent = 0);

	/*!
	 * \brief Construct an Edit Loop Dialog
	 * \param subject The Loop to edit
	 * \param parent The parent widget of the dialog
	 */
	explicit LoopDialog(const Loop &subject, QWidget *parent = 0);

	/// Spawn a dialog for editing the Loop whose id is passed as a parameter
	static void editLoop(int priority);

private:
	/// Initialise the parts of the dialog that are common to both constructors
	void initDialog();
};

#endif // LOOPDIALOG_H
