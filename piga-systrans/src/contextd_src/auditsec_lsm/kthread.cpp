#include "kthread.h"


KThread::KThread(KernelContext * kc) : KC(kc)
{

}


void KThread::run()
{
	while(auditsec_question(KC->usai()) == 0){
		switch (KC->usai()->type){
			case AUDITSEC_FILE:
				switch (context_changed(//"pid", usai->pid,
					"fullpath", usai->auditsec_struct.file.fullpath,
			//					"filename", usai->auditsec_struct.file.name,
			NULL, NULL)){
					case CONTEXT_ACCEPTED:
						auditsec_answer(true);
						std::cout << "Transition acceptée." << std::endl;
						break;
					case CONTEXT_REFUSED:
						auditsec_answer(false);
						std::cerr << "Transition refusée." << std::endl;
						break;
					case CONTEXT_ERROR:
						auditsec_answer(false);
						std::cerr << "Erreur dans la transition : " << context_getLastError() << std::endl;
						break;
					default:
						auditsec_answer(false);
						std::cerr << "Default ! On ne devrait pas être là !" << std::endl;
						break;
			}
			#ifdef DEBUG
			read_execpath(usai->pid, exec_path);
			std::cout << "AuditSec, file access: " << usai->auditsec_struct.file.fullpath
			<< "/" << usai->auditsec_struct.file.name << ", pid: " << usai->pid << ", execname: "
			<< exec_path /*<< ", mask: " << usai->auditsec_struct.file.mask*/ << std::endl;
			#endif /* DEBUG */
			break;

					case AUDITSEC_DIR:
						switch (context_changed("pid", usai->pid,
							"fullpath", usai->auditsec_struct.dir.fullpath,
			  NULL, NULL)){
							case CONTEXT_ACCEPTED:
								auditsec_answer(true);
								std::cout << "Transition acceptée." << std::endl;
								break;
							case CONTEXT_REFUSED:
								auditsec_answer(false);
								std::cerr << "Transition refusée." << std::endl;
								break;
							case CONTEXT_ERROR:
								auditsec_answer(false);
								std::cerr << "Erreur dans la transition : " << context_getLastError() << std::endl;
								break;
							default:
								auditsec_answer(false);
								std::cerr << "Default ! On ne devrait pas être là !" << std::endl;
								break;
			  }
			  #ifdef DEBUG
			  read_execpath(usai->pid, exec_path);
			  std::cout << "AuditSec, mkdir: " << usai->auditsec_struct.dir.fullpath
			  << ", pid: " << usai->pid << ", execname: " << exec_path << usai->execname /*<< ", mode: "
			  << usai->auditsec_struct.dir.mode*/ << std::endl;
			  #endif /* DEBUG */
			  break;
							default:
								std::cerr << "AuditSec, can't determine struct type !" << std::endl;
								auditsec_answer(false);
								break;
		}
    }
}
