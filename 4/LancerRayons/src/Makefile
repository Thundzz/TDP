###########################################################################
#                       Lanceur de rayons simplifie                       #
###########################################################################
# ENSERB, 3me annee Informatique                               1995-1996  #
# Option AAMP, Module MP2                                                 #
#                                                                         #
###########################################################################
# Author : Gilles Subrenat                                                #
# Date   : 15 janvier 1996                                                #
#                                                                         #
# $Id$
###########################################################################/

# make -p pour avoir les macro et les regles definies 
#         dans /usr/share/lib/make/make.rules
# make -P donne les dependances des differentes cibles


# repertoire d'installation
BIN_DIR = ../bin

# mode sans test unitaire
#DEBUG = -DNDEBUG

# macro definissant ou se trouve la commande zcat
ZCAT = -DZCAT_COMMAND=\"`which zcat`\"

# Macros predefinies
CC = gcc
CC = mpicc
MPICC = mpicc

# options de compilation
CPPFLAGS = -I../include -I. $(DEBUG) $(ZCAT) -pthread
#CFLAGS = -O -Wall
CFLAGS = -g -Wall
LDLIBS = -lm

# application : fichier executable
LANCEUR_OBJ = lanceur.o
LANCEUR_JOUET_OBJ = lanceur_jouet.o
BIN = lanceur lanceur_jouet lanceur_static

# tests de modules locaux.
TEST= test_queue.out

#pour changer de type de binder, mettre le bon .c
#SRC_BINDER = bnd.basic.c
SRC_BINDER = bnd.voxel.c

SRC_ALL_BINDER = bnd.basic.c  bnd.voxel.c 

HDR_BINDER = bnd.h


#pour changer de type de camera, mettre le bon .c
SRC_CAMERA = cmr.basic.c

SRC_ALL_CAMERA = cmr.basic.c

HDR_CAMERA = cmr.h


#pour changer de type de lancer de rayons, mettre le bon .c
#SRC_IMAGE = img.basic.c
SRC_IMAGE_STATIC = img.mpi-static.c
#SRC_IMAGE = img.mpi-thread.c
SRC_IMAGE = img.mpi-dyn.c
HDR_IMAGE = $(SRC_IMAGE:.c=.h)

#pour changer de type de lancer de rayons, mettre le bon .c
SRC_ALL_IMAGE = img.basic.c

#pour changer de type de calcul d'illumination, mettre le bon .c
SRC_SCENE = scn.image.c

SRC_ALL_SCENE = scn.image.c

HDR_SCENE = $(SRC_SCENE:.c=.h)


#tous les types geometrique
SRC_GEO = geo.c geo.nappe.c geo.point.c geo.quadric.c

HDR_GEO = $(SRC_GEO:.c=.h)


#entites
SRC_ETT = ett.c

HDR_ETT = $(SRC_ETT:.c==.h)


#gestion des classes
SRC_CLASS = class.c

HDR_CLASS = $(SRC_CLASS:.c=.h)


# sources du repertoire
SRC_ALL = $(BIN:=.c) $(SRC_ALL_BINDER) $(SRC_ALL_CAMERA) $(SRC_ALL_IMAGE) $(SRC_SCENE) $(SRC_GEO) $(SRC_ETT) $(SRC_CLASS) getarg.c read_file.c

# sources pour fabriquer l'executable
SRC = $(SRC_BINDER) $(SRC_CAMERA) $(SRC_SCENE) $(SRC_GEO) $(SRC_ETT) $(SRC_CLASS) getarg.c read_file.c

# fichier objet cree
OBJ = $(SRC:.c=.o)

# cibles standards
all: $(BIN) $(TEST)


#
clean:
	$(RM) *.o *~ *.out

realclean: clean
	$(RM) $(BIN)

install: all
	cp $(BIN) $(BIN_DIR)

uninstall:
	cd $(BIN_DIR) ;	$(RM) $(BIN)

lanceur_jouet: $(LANCEUR_JOUET_OBJ) queue.o $(OBJ) $(SRC_IMAGE:.c=.o)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

lanceur_static: $(LANCEUR_OBJ) queue.o $(OBJ) $(SRC_IMAGE_STATIC:.c=.o)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

lanceur: $(LANCEUR_OBJ) queue.o $(OBJ) $(SRC_IMAGE:.c=.o)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

test_queue.out: test_queue.c queue.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
.c.o :
	$(CC) $(CPPFLAGS) $(CFLAGS)  -c $<


#$(OBJ): $(HDR)
#	$(CC) $(CPPFLAGS) $(CFLAGS) $*.c -c


lanceur.o: getarg.h
lanceur.o: ../include/exit-if.h ../include/const.h ../include/macro.h
lanceur.o: ../include/type.h lanceur.h read_file.h bnd.h
lanceur.o: ../include/type_spec.h ett.h geo.h scn.h img.h
lanceur.o: lanceur_jouet.h
bnd.basic.o: ../include/macro.h
bnd.basic.o: ../include/exit-if.h ../include/const.h
bnd.basic.o: ../include/type.h ../include/type_spec.h ett.h geo.h class.h
bnd.basic.o: lanceur.h bnd.h
bnd.voxel.o: ../include/macro.h
bnd.voxel.o: ../include/exit-if.h ../include/const.h
bnd.voxel.o: ../include/type.h ../include/type_spec.h ett.h geo.h class.h
bnd.voxel.o: lanceur.h bnd.h 
cmr.basic.o: cmr.h ../include/type.h
cmr.basic.o: ../include/type_spec.h ett.h geo.h ../include/macro.h
cmr.basic.o: ../include/exit-if.h ../include/const.h
img.basic.o: img.h
img.basic.o: ../include/type.h ../include/macro.h 
img.basic.o: ../include/exit-if.h ../include/const.h ../include/type_spec.h
img.basic.o: ett.h geo.h lanceur.h scn.h bnd.h cmr.h
scn.image.o: ../include/macro.h
scn.image.o: ../include/exit-if.h
scn.image.o: ../include/const.h ../include/type.h ../include/type_spec.h
scn.image.o: ett.h geo.h scn.h bnd.h lanceur.h class.h
geo.o: geo.h 
geo.o: ../include/type.h
geo.nappe.o: ../include/macro.h 
geo.nappe.o: ../include/exit-if.h ../include/type.h ../include/type_spec.h
geo.nappe.o: ett.h geo.h lanceur.h geo.nappe.h
geo.point.o: ../include/const.h
geo.point.o: ../include/macro.h 
geo.point.o: ../include/exit-if.h ../include/type.h ../include/type_spec.h
geo.point.o: ett.h geo.h geo.point.h
geo.quadric.o: ../include/const.h
geo.quadric.o: ../include/macro.h 
geo.quadric.o: ../include/exit-if.h ../include/type.h ../include/type_spec.h
geo.quadric.o: ett.h geo.h lanceur.h geo.quadric.h
ett.o: ../include/exit-if.h ../include/macro.h
ett.o: ../include/type.h ../include/const.h class.h ../include/type_spec.h
ett.o: ett.h geo.h
class.o: class.h 
class.o: ../include/type.h ../include/type_spec.h ett.h geo.h geo.nappe.h
class.o: geo.quadric.h geo.point.h
getarg.o: getarg.h
read_file.o: bnd.h img.h cmr.h read_file.h

#Add compile rules for the static MPI version
img.mpi-static.o: ../include/type.h ../include/macro.h 
img.mpi-static.o: ../include/exit-if.h ../include/const.h ../include/type_spec.h
img.mpi-static.o: ett.h geo.h lanceur.h scn.h bnd.h cmr.h

#Add compile rules for the threaded MPI version
img.mpi-thread.o: ../include/type.h ../include/macro.h 
img.mpi-thread.o: ../include/exit-if.h ../include/const.h ../include/type_spec.h
img.mpi-thread.o: ett.h geo.h lanceur.h scn.h bnd.h cmr.h

#Add compile rules for the threaded MPI version
img.mpi-dyn.o: ../include/type.h ../include/macro.h 
img.mpi-dyn.o: ../include/exit-if.h ../include/const.h ../include/type_spec.h
img.mpi-dyn.o: ett.h geo.h lanceur.h scn.h bnd.h cmr.h lanceur_jouet.h