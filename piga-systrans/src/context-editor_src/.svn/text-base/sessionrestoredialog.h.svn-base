#ifndef SESSIONRESTOREDIALOG_H
#define SESSIONRESTOREDIALOG_H

#include <QMessageBox>

/// A dialog asking if the last saved session should be restored (using a backup of the last edited transitions)
class SessionRestoreDialog : public QMessageBox
{
    Q_OBJECT
public:
	/*!
	 * \brief Construct a Session Restore Dialog object
	 * \param tmp_file_path The path to the last session's transition backup
	 * \param parent The parent widget of the dialog
	 */
	explicit SessionRestoreDialog(QString tmp_file_path, QWidget *parent = 0);

private:
	QString _tmp_file_path;

private slots:
	/// Delete the backup file and reject the dialog
	void onDeleteClicked();

	/// Open a file dialog asking where to move the backup file, then reject if the file is properly moved
	void onMoveClicked();

	/// Restore the session, delete the backup file and accept the dialog
	void onRestoreClicked();

};

#endif // SESSIONRESTOREDIALOG_H
