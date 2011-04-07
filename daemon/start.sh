<<<<<<< HEAD
#!/bin/bash

if [ $# -ne 2 ] ; then
	echo "usage: $0 pid_contextd pid_contxt-nofify";
	exit 1;
fi

=======
>>>>>>> 5ebf9d3dddf9735f4c50f1aaf721abc440b4613f
LD_LIBRARY_PATH="../piga-systrans/build/src/" ./daemon $1 $2
