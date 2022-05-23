/*
	Autor: Luis Otavio Rigo Junior
	Objetivo: 
		Este arquivo destina-se a implementacao das estrategias de jogo dos agentes.
	
	Devem ser implementados os 4 prototipos:
		init_Player1 - executada uma unica vez e contem as funcoes de inicializacao da estrategia do jogador 1;
		run_Player1 - executado a cada passo do jogo e retorna uma string com a direcao de movimento do jogador 1;
		init_Player2 - executada uma unica vez e contem as funcoes de inicializacao da estrategia do jogador 2;
		run_Player2 - executado a cada passo do jogo e retorna uma string com a direcao de movimento do jogador 2.
	
	Funcoes que podem ser chamadas pelo jogador (agente):
		char *maze_VerAmbiente(char tipo[MAXLINE]);
			- Utilizada para verificar alguma informacao da celula. Ex.: id;
		bool maze_VerCaminho(const char *direcao);
			- Retorna se existe um caminho naquela direcao (verdadeiro ou falso).
		bool maze_VerMinotauro(const char *direcao);
			- Retorna se o minotauro estah naquela direcao (verdadeiro ou falso).
		float maze_CustoDoCaminho(const char *direcao);
			- Retorna o custo de executar um passo naquela direcao.
		float maze_HeuristicaDistEuclidiana(const char *direcao);
			- Retorna o a distancia heuclidiana da celula que estah naquela direcao ateh a saida.
	
	Constantes que podem ser usadas pelo jogador (agente):
		#define NUMCAMINHOS 4
		const char *id_Caminhos[NUMCAMINHOS] = {"norte", "sul", "oeste", "leste"};
		const char *id_Retornos[NUMCAMINHOS] = {"sul", "norte", "leste", "oeste"};
		typedef struct str_PosicaoPlano {
			int x,y;
		} tipo_PosicaoPlano;
*/

#include <limits.h>

//Matriz e struct de posicoes do agente 1
int player1_matrix[MAXCEL][MAXCEL];
tipo_PosicaoPlano posicaoPlayer1;

//Matriz e struct de posicoes do agente 2
int player2_matrix[MAXCEL][MAXCEL];
tipo_PosicaoPlano posicaoPlayer2;
//Pilha para armazenamento do caminho percorrido pelo player2
std::vector<const char *> pilhaPlayer2_historico;

// *** 	FUNCOES DE INICIALIZACAO E EXECUCAO DO JOGADOR 1 ***
//	Implementacao da primeira estrategia de jogo.
//	Estratégia: em cada posição do agente, buscar todos os caminhos possiveis. Em seguida buscar em qual caminho foi passado menos vezes pelo agente 1
void init_Player1() {
	//Inicializa a matriz do player 1 com valor 0 nos indices
	for(int x=0; x<MAXCEL; x++){
		for(int y=0; y<MAXCEL; y++){
			player1_matrix[x][y] = 0;
		}
	}

	//Define posicao atual do agente 1
	posicaoPlayer1.x = 0;
	posicaoPlayer1.y = 0;
}

//Funcao que verifica o melhor caminho a ser seguido (caminho passado menos vezes pelo agente)
int escolherMelhorCaminho(int *vet_Caminhos){
	int i, qtdPassos = 0, minPassos = INT_MAX,caminho;
	for(i=0; i<NUMCAMINHOS; i++){
		//Caso seja um caminho válido (!=PAREDE)
		if(vet_Caminhos[i] == 1){
			//Atualiza o valor de qtdPassos com o valor da posicao que seria seguida usando id_Caminhos[i]
			switch(i){
				case 0:
					qtdPassos = player1_matrix[posicaoPlayer1.x-1][posicaoPlayer1.y];
					break;
				case 1:
					qtdPassos = player1_matrix[posicaoPlayer1.x+1][posicaoPlayer1.y];
					break;
				case 2:
					qtdPassos = player1_matrix[posicaoPlayer1.x][posicaoPlayer1.y-1];
					break;
				case 3:
					qtdPassos = player1_matrix[posicaoPlayer1.x][posicaoPlayer1.y+1];
					break;
				default:
					break;
			}
			//Verifica qual valor é menor e retorna o indice do vetor vet_Caminhos com o melhor caminho a ser seguido
			if(qtdPassos < minPassos){
				minPassos = qtdPassos;
				caminho = i;
			}
		}
	}
	return caminho;
}

const char *run_Player1() {
	const char *movimento;

	//Verifica quais caminhos sao possiveis. Ex: se o unico caminho é para leste -> vet_Caminhos=[0,0,0,1]
	int vet_Caminhos[NUMCAMINHOS];
	int cont_caminhos;

	for(cont_caminhos = 0; cont_caminhos<NUMCAMINHOS; cont_caminhos++){
		if(maze_VerCaminho(id_Caminhos[cont_caminhos]) == PAREDE) vet_Caminhos[cont_caminhos] = 0;
		else vet_Caminhos[cont_caminhos] = 1;
	}

	//Busca o melhor caminho com o vetor de caminhos possiveis vet_Caminhos
	int caminho = escolherMelhorCaminho(vet_Caminhos);
	
	//Verifica qual caminho a ser seguido
	switch(caminho){
		case 0:
			posicaoPlayer1.x--;
			break;
		case 1:
			posicaoPlayer1.x++;
			break;
		case 2:
			posicaoPlayer1.y--;
			break;
		case 3:
			posicaoPlayer1.y++;
			break;
		default:
			break;
	}
	//Atualiza posicao do agente 1
	player1_matrix[posicaoPlayer1.x][posicaoPlayer1.y]++;

	movimento = id_Caminhos[caminho];
	
	return movimento;
}

// *** 	FUNCOES DE INICIALIZACAO E EXECUCAO DO JOGADOR 2 ***
//	Implementacao da segunda estrategia de jogo.
// 	Estratégia utilizada: buscar um caminho com valor -1 e seguir adiante, caso n for -1, busca um caminho aleatório
void init_Player2() {
	//Inicializa a matriz do player 1 com valores -1
	for(int x=0; x<MAXCEL; x++){
		for(int y=0; y<MAXCEL; y++){
			player2_matrix[x][y] = -1;
		}
	}
	
	//Inicializando a pilha (vazia)
	while(pilhaPlayer2_historico.size() > 0){
		pilhaPlayer2_historico.pop_back();
	}

	//Define posicao atual do agente 2
	posicaoPlayer2.x = 0;
	posicaoPlayer2.y = 0;
	player2_matrix[posicaoPlayer2.x][posicaoPlayer2.y]++;
}
const char *run_Player2() {
	const char *movimento = "oeste";
	
	// //Verifica se algum caminho é válido (!=PAREDE) e valor da proxima posicao igual a -1	
	// if (maze_VerCaminho("norte") == CAMINHO && player2_matrix[posicaoPlayer2.x-1][posicaoPlayer2.y]==-1)  {
	// 	posicaoPlayer2.x-=1;
	// 	player2_matrix[posicaoPlayer2.x][posicaoPlayer2.y]=0;
	// 	movimento = "norte";
	// }
	// else if (maze_VerCaminho("sul") == CAMINHO && player2_matrix[posicaoPlayer2.x+1][posicaoPlayer2.y]==-1)  {
	// 	posicaoPlayer2.x+=1;
	// 	player2_matrix[posicaoPlayer2.x][posicaoPlayer2.y]=0;
	// 	movimento = "sul";
	// }
	// else if (maze_VerCaminho("oeste") == CAMINHO && player2_matrix[posicaoPlayer2.x][posicaoPlayer2.y-1]==-1)  {
	// 	posicaoPlayer2.y-=1;
	// 	player2_matrix[posicaoPlayer2.x][posicaoPlayer2.y]=0;
	// 	movimento = "oeste";
	// }
	// else if (maze_VerCaminho("leste") == CAMINHO && player2_matrix[posicaoPlayer2.x][posicaoPlayer2.y+1]==-1) {
	// 	posicaoPlayer2.y+=1;
	// 	player2_matrix[posicaoPlayer2.x][posicaoPlayer2.y]=0;
	// 	movimento = "leste";
	// }
	// //Caso contrário
	// else {
	// 	int move = rand()%4;//Sorteia um numero
	// 	//Verifica se o caminho e valido
	// 	switch(move){
	// 		case 0:
	// 			if (maze_VerCaminho("norte") == CAMINHO) {
	// 				posicaoPlayer2.x-=1;
	// 				movimento = "norte";
	// 			}
	// 			break;
	// 		case 1:
	// 			if(maze_VerCaminho("sul") == CAMINHO) {
	// 				posicaoPlayer2.x+=1;
	// 				movimento = "sul";
	// 			}
	// 			break;
	// 		case 2:
	// 			if(maze_VerCaminho("oeste") == CAMINHO) {
	// 				posicaoPlayer2.y-=1;
	// 				movimento = "oeste";
	// 			}
	// 			break;
	// 		case 3:
	// 			if(maze_VerCaminho("leste") == CAMINHO){
	// 				posicaoPlayer2.y+=1;
	// 				movimento = "leste";
	// 			}
	// 			break;
	// 		default:
	// 			break;
	// 	}
	// 	//Atualiza posicao do agente 2
	// 	player2_matrix[posicaoPlayer2.x][posicaoPlayer2.y]++;
	// }
	return movimento;
}
