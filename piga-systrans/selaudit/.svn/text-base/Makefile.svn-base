CWD = $(shell pwd)

install: 
	ln -s $(CWD)/selaudit.py /usr/local/bin/
	ln -s $(CWD)/tagging/seltag_app.py /usr/local/bin/
	ln -s $(CWD)/tagging/seltag_compare.py /usr/local/bin/
	cp $(CWD)/selaudit_user.sh /usr/local/bin/

uninstall:
	rm /usr/local/bin/selaudit.py
	rm /usr/local/bin/seltag_app.py
	rm /usr/local/bin/seltag_compare.py
	rm /usr/local/bin/selaudit_user.sh
