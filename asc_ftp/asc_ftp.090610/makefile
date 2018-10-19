ADD_64BIT_OPT=+DD64 -DSS_64BIT_SERVER

all:clean_screen billing_ftp make_finish
billing_ftp:ftp_main.c ftp_main.h ftp_function.o 
	@cc $(ADD_64BIT_OPT) -obilling_ftp -g ftp_main.c ftp_function.o ftp_public.o ftp_net.o
ftp_function.o:ftp_function.c ftp_function.h ftp_public.o ftp_net.o
	@cc $(ADD_64BIT_OPT) -c -g -oftp_function.o ftp_function.c
ftp_public.o:ftp_public.c ftp_public.h
	@cc $(ADD_64BIT_OPT) -c -g -oftp_public.o ftp_public.c
ftp_net.o:ftp_net.c ftp_net.h
	@cc $(ADD_64BIT_OPT) -c -g -oftp_net.o ftp_net.c
clean:
	@rm *.o
clean_screen:
	@clear
	@echo "**********************************************************************"
	@echo "*--------------------------------------------------------------------*"
	@echo "*"
	@echo "*       Copyright (C), 1995-2003, Si-Tech Information Technology Ltd."
	@echo "*"
	@echo "**********************************************************************"
	@echo "*"
	@echo "*       File_name:      ftp_net.c"
	@echo "*       Author:         ’≈∆ÊŒ∞"
	@echo "*       Version:        ∞Ê±æ£®V1.00£©"
	@echo "*       Date:           2003/11/15"
	@echo "*"
	@echo "**********************************************************************"
	@echo 
	@echo 
	@echo 
make_finish:
	@echo "**********************************************************************"
	@echo "*--------------------------------------------------------------------*"
	@echo "        Make progress finished !!!"
	@echo "**********************************************************************"
