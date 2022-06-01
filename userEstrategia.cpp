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

using namespace std;

#include <float.h>
#include <limits.h>
#include <stack>
#include <string>

/* ===========================================================================
   ============================ AGENTE 1 =====================================
   ===========================================================================
*/
//Matriz e struct de posicoes do agente 1
int player1_matrix[60][35] = {0};
tipo_PosicaoPlano posicaoPlayer1;
//Pilha para armazenamento do caminho percorrido pelo player2
stack <const char *> pilhaPlayer1_caminho;


/* ===========================================================================
   ============================ AGENTE 2 =====================================
   ===========================================================================
*/
//Matriz e struct de posicoes do agente 2
int player2_matrix[60][35] = {0};
tipo_PosicaoPlano posicaoPlayer2;
//Pilha para armazenamento do caminho percorrido pelo player2
stack <const char *> pilhaPlayer2_caminho;
//Flag para determinar se o player vai fugir ou nao do minotauro
bool isEscapingPlayer2;
//Inteiro armazenando a quantidade de passos para fugir do minotauro
int qtdPassosFuga;

/* ===========================================================================
   ======================= IMPLEMENTACAO PLAYER 1 ============================
   ===========================================================================
*/

// *** 	FUNCOES DE INICIALIZACAO E EXECUCAO DO JOGADOR 1 ***
//	Implementacao da primeira estrategia de jogo.
//	Estratégia: em cada posição do agente, buscar todos os caminhos possiveis. Em seguida buscar em qual caminho foi passado menos vezes pelo agente 1
void init_Player1() {
	// //Inicializando a pilha
	// while(pilhaPlayer1_caminho.size() > 0){
	// 	pilhaPlayer1_caminho.pop();
	// }
	//Define posicao atual do agente 1
	posicaoPlayer1.x = 0;
	posicaoPlayer1.y = 0;
}

//Funcao que verifica o melhor caminho a ser seguido (caminho passado menos vezes pelo agente)
int escolheCaminhoPlayer1(int *vet_Caminhos){
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
	const char *movimento = "null";

	//Verifica quais caminhos sao possiveis. Ex: se o unico caminho é para leste -> vet_Caminhos=[0,0,0,1]
	int vet_Caminhos[NUMCAMINHOS] = {0};
	int i;

	for(i = 0; i<NUMCAMINHOS; i++){
		if(maze_VerCaminho(id_Caminhos[i]) == CAMINHO) vet_Caminhos[i] = 1;
	}

	//Busca o melhor caminho com o vetor de caminhos possiveis vet_Caminhos
	int caminho = escolheCaminhoPlayer1(vet_Caminhos);
	
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

/* ===========================================================================
   ======================= IMPLEMENTACAO PLAYER 2 ============================
   ===========================================================================
*/

/* 
	*** 	FUNCOES DE INICIALIZACAO E EXECUCAO DO JOGADOR 2 ***
	Implementacao da segunda estrategia de jogo.
 	Estratégia utilizada: percorrer o labirinto atualizando a posicao atual para 1 (inicialmente tudo 0) e empilhando a direcao
                          percorrido na pilha. Verifica se existe caminho ainda nao percorrido e escolhe esse como proximo
 						  passo. Caso todas as direcoes ja foram percorridas, volta o caminho seguindo a direcao oposta do topo
 						  da pilha e desempilha em seguida. 
*/
void init_Player2() {	
	//Inicializando a pilha (vazia)
	while(pilhaPlayer2_caminho.size() > 0){
		pilhaPlayer2_caminho.pop();
	}

	//Define posicao atual do agente 2
	posicaoPlayer2.x = 0;
	posicaoPlayer2.y = 0;

	isEscapingPlayer2 = false;
	qtdPassosFuga = 0;
}

/*
	Funcao que dado um vetor com os caminhos possiveis de serem percorridos, verifica se um desses é viavel de seguir (se for um
	caminho ainda não percorrido) e retorna o indice do caminho escolhido. Caso todos ja foram percorridos, retorna -1. O caminho
	que eh retornado possui a menor heuristica (calculada pela soma do custo do caminho + a distancia euclidiana)
*/
int escolheCaminhoPlayer2(int *caminhosPossiveis){
	float distEuclidiana = FLT_MAX, funcHeuristica;
	int i, caminho = -1;

	for(i=0; i<NUMCAMINHOS; i++){
		if(caminhosPossiveis[i] != 0){
			switch (i)
			{
				case 0:
					// Visualiza se a direcao norte é viavel
					if(player2_matrix[posicaoPlayer2.x][posicaoPlayer2.y-1] == 0) {
						funcHeuristica = maze_CustoDoCaminho(id_Caminhos[i]) + maze_HeuristicaDistEuclidiana(id_Caminhos[i]);
						if(funcHeuristica < distEuclidiana){ caminho = i; distEuclidiana = funcHeuristica;}
					}
					break;
				case 1:
					// Visualiza se a direcao sul é viavel
					if(player2_matrix[posicaoPlayer2.x][posicaoPlayer2.y+1] == 0){	
						funcHeuristica = maze_CustoDoCaminho(id_Caminhos[i]) + maze_HeuristicaDistEuclidiana(id_Caminhos[i]);
						if(funcHeuristica < distEuclidiana){ caminho = i; distEuclidiana = funcHeuristica;}
					}
					break;
				case 2:
					// Visualiza se a direcao oeste é viavel
					if(player2_matrix[posicaoPlayer2.x-1][posicaoPlayer2.y] == 0){
						funcHeuristica = maze_CustoDoCaminho(id_Caminhos[i]) + maze_HeuristicaDistEuclidiana(id_Caminhos[i]);
						if(funcHeuristica < distEuclidiana){ caminho = i; distEuclidiana = funcHeuristica;}
					}
					break;
				case 3:
					// Visualiza se a direcao leste é viavel
					if(player2_matrix[posicaoPlayer2.x+1][posicaoPlayer2.y] == 0){
						funcHeuristica = maze_CustoDoCaminho(id_Caminhos[i]) + maze_HeuristicaDistEuclidiana(id_Caminhos[i]);
						if(funcHeuristica < distEuclidiana) { caminho = i; distEuclidiana = funcHeuristica;}
					}
					break;
				default:
					break;
			}
		}
	}
	// Nenhum caminho viavel
	return caminho;
}

/*
	Dado uma direcao como string retorna o indice segundo o vetor id_Caminhos[]
*/
int get_id_return_path(const char *direction) {
	if(direction == "norte") return 0;
	else if(direction == "sul") return 1;
	else if(direction == "oeste") return 2;
	
	return 3;
}

const char *andarPlayer2(int caminho){

	player2_matrix[posicaoPlayer2.x][posicaoPlayer2.y] = 1;

	const char * direcao = id_Caminhos[caminho];

	pilhaPlayer2_caminho.push(direcao); // Empilha a direção

	switch(caminho){
		case 0:
			posicaoPlayer2.y--;
			break;
		case 1:
			posicaoPlayer2.y++;
			break;
		case 2:
			posicaoPlayer2.x--;
			break;
		case 3:
			posicaoPlayer2.x++;
			break;
		default:
			break;
	}

	return direcao;
}

const char *retrocederPlayer2(){
	const char *top_stack = "null";
	top_stack = pilhaPlayer2_caminho.top(); // Armazenando o topo da pilha
	pilhaPlayer2_caminho.pop(); // Desempilhando

	int caminho_retorno = get_id_return_path(top_stack);

	switch(caminho_retorno){
		case 0:
			posicaoPlayer2.y++;
			break;
		case 1:
			posicaoPlayer2.y--;
			break;
		case 2:
			posicaoPlayer2.x++;
			break;
		case 3:
			posicaoPlayer2.x--;
			break;
		default:
			break;
	}

	return id_Retornos[caminho_retorno];
}

const char *fugirPlayer2(){
	player2_matrix[posicaoPlayer2.x][posicaoPlayer2.y] = 0;

	const char *top_stack = "null";
	top_stack = pilhaPlayer2_caminho.top(); // Armazenando o topo da pilha
	pilhaPlayer2_caminho.pop(); // Desempilhando

	int caminho_retorno = get_id_return_path(top_stack);

	switch(caminho_retorno){
		case 0:
			posicaoPlayer2.y++;
			break;
		case 1:
			posicaoPlayer2.y--;
			break;
		case 2:
			posicaoPlayer2.x++;
			break;
		case 3:
			posicaoPlayer2.x--;
			break;
		default:
			break;
	}

	return id_Retornos[caminho_retorno];
}

bool isBeginPlayer2(){
	return (posicaoPlayer2.x==0 && posicaoPlayer2.y==0) ? true : false;
}

const char *run_Player2() {
	const char *movimento = "null";
	int caminhoEscolhido;

	if(!isEscapingPlayer2){
		player2_matrix[posicaoPlayer2.x][posicaoPlayer2.y] = 1; // Atualiza a posicao atual como percorrida
		
		int caminhosPossiveis[NUMCAMINHOS] = {0};
		int i;
		for(i = 0; i<NUMCAMINHOS; i++){
			if(maze_VerCaminho(id_Caminhos[i]) == CAMINHO){
				caminhosPossiveis[i] = 1;
			}
		}
		
		caminhoEscolhido = escolheCaminhoPlayer2(caminhosPossiveis);

		if(caminhoEscolhido != -1){
			
			isEscapingPlayer2 = maze_VerMinotauro(id_Caminhos[caminhoEscolhido]);
		}
	}

	if(isEscapingPlayer2){
		if(isBeginPlayer2()){
			qtdPassosFuga = 0;
			isEscapingPlayer2 = false;
		}
		//Fugir
		else if(qtdPassosFuga < 30){
			qtdPassosFuga++;
			movimento = fugirPlayer2();
		}else{
			qtdPassosFuga = 0;
			isEscapingPlayer2 = false;
		}
	}else{
		//Nao fugir
		//Verificar se a pilha de fuga está vazia
		//	Se pilha fuga ainda nao vazia, voltar fuga
		if(qtdPassosFuga > 0){
			qtdPassosFuga = 0;
		}

		if(caminhoEscolhido == -1){
			// Nao há caminhos ainda nao percorridos, entao é momento de desempilhar
			movimento = retrocederPlayer2();

		}else{
			// Encontrou pelo menos um caminho viavel
			movimento = andarPlayer2(caminhoEscolhido);
		}
	}

	return movimento;
}