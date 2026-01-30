FLAGS = -Wall -I$(INC_DIR)
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
LIB_DIR = lib
BIN_DIR = bin
LDFLAGS = -L$(LIB_DIR) -lDial -lRepReq -lInet -lUsers

all: setup clean $(LIB_DIR)/libInet.a $(LIB_DIR)/libDial.a $(LIB_DIR)/libRepReq.a $(LIB_DIR)/libUsers.a game gameClient gameServer

# ----- Librairie statique -----
$(LIB_DIR)/libInet.a: $(OBJ_DIR)/data.o $(OBJ_DIR)/session.o
	ar qvs $@ $^

$(LIB_DIR)/libRepReq.a: $(OBJ_DIR)/libRepReq.o
	ar qvs $@ $^

$(LIB_DIR)/libDial.a: $(OBJ_DIR)/libDial.o 
	ar qvs $@ $^

$(LIB_DIR)/libUsers.a: $(OBJ_DIR)/users.o 
	ar qvs $@ $^


# ----- Fichiers objets -----

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	gcc -c $< -o $@ $(FLAGS)

# ----- Exécutables -----
game: $(SRC_DIR)/game.c $(LIB_DIR)/libInet.a $(LIB_DIR)/libDial.a $(LIB_DIR)/libRepReq.a $(LIB_DIR)/libUsers.a
	gcc $< -o $(BIN_DIR)/$@ $(FLAGS) $(LDFLAGS)

gameClient: $(SRC_DIR)/game.c $(LIB_DIR)/libInet.a $(LIB_DIR)/libDial.a $(LIB_DIR)/libRepReq.a $(LIB_DIR)/libUsers.a
	gcc $< -o $(BIN_DIR)/$@ $(FLAGS) $(LDFLAGS) -DCLIENT

gameServer: $(SRC_DIR)/game.c $(LIB_DIR)/libInet.a $(LIB_DIR)/libDial.a $(LIB_DIR)/libRepReq.a $(LIB_DIR)/libUsers.a
	gcc $< -o $(BIN_DIR)/$@ $(FLAGS) $(LDFLAGS) -DSERVER

# ----- Nettoyage -----
clean:
	rm -f $(OBJ_DIR)/* $(LIB_DIR)/* $(BIN_DIR)/*

# Création des dossiers si nécessaire
setup:
	mkdir -p $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR)



