#include <iostream>
#include <conio.h>
#include <locale.h>
#include <vector>
#include <ctime>

#define NOMINMAX
#include <windows.h>

#include <string>
#include <io.h>
#include <fcntl.h>
#include <sstream>
#include <limits>

#include "sqlite3.h" // https://www.sqlite.org/download.html

using namespace std;

struct Produto {
	int id = 0;
	string nome;
	float preco = 0.0;
};

struct Cliente {
	int id = 0;
	string nome;
};

struct Data {
	int dia;
	int mes;
	int ano;
};

struct Venda {
	int cliente_id = 0;
	int produto_id = 0;
	int quant_prod = 0;
	int devolucao = 0;
	Data data = { 0, 0, 0 };
};

sqlite3* db = nullptr; // Banco de dados global para facilitar o acesso em todas as funções

int opc = 0; // Variável global para controlar cursor no menu
int menu = 1; // Variável global para controlar qual menu está sendo exibido (1 = principal, 2 = relatórios)

void printMenu();
void printMenuRelatorios();
void printMenuInicial();

void inserir_venda(sqlite3* db, Venda venda);
void inserir_cliente(sqlite3* db, Cliente cliente);
void inserir_produto(sqlite3* db, Produto produto);

void listar_produtos(sqlite3* db);
void listar_clientes(sqlite3* db);
void listar_vendas(sqlite3* db);

void registrar_produto();
void registrar_cliente();
void registrar_venda();

void relatorio_diario(sqlite3* db);
void relatorio_mensal(sqlite3* db);
void relatorio_anual(sqlite3* db);

void resetar_banco(sqlite3* db);
void gerar_dados_teste(sqlite3* db);

int banco_de_dados();
int readKey();

string trim(const string& str);
string utf8_from_wstring(const wstring& w);
wstring wstring_from_utf8(const string& str);

int main()
{
	// Habilita leitura wide (UTF-16) no stdin para usar std::wcin
	setlocale(LC_ALL, "");

	_setmode(_fileno(stdin), _O_U16TEXT);
	_setmode(_fileno(stdout), _O_U16TEXT);

	int rc = banco_de_dados(); // Configura e abre o banco de dados, criando as tabelas

	if (rc != SQLITE_OK) {
		// Se houve erro, tenta fechar a conexão caso esteja aberta e encerra
		if (db) sqlite3_close(db);

		return 1;
	}

	menu = 1;
	// Loop do programa, mostrando o menu e processando as opções
	while (1)
	{
		printMenu();

		switch (readKey())
		{
			case 13:
			{
				if (opc == 0) {

					if (menu == 1) {
						sqlite3_close(db);
						return 0;
					}
					else if (menu == 2) {
						menu = 1;
						opc = 0;
					}
					
				}
				else if (opc == 1) {

					if (menu == 1) registrar_produto();
					else if (menu == 2) relatorio_diario(db); // Exemplo: relatório diário para 01/01/2026
				}
				else if (opc == 2) {

					if (menu == 1) listar_produtos(db);
					else if (menu == 2) relatorio_mensal(db); // Exemplo: relatório mensal para janeiro de 2026
				}
				else if (opc == 3) {

					if (menu == 1) registrar_cliente();
					else if (menu == 2) relatorio_anual(db); // Exemplo: relatório anual para 2026
				}
				else if (opc == 4) {

					listar_clientes(db);
				}
				else if (opc == 5) {

					registrar_venda();
				}
				else if (opc == 6) {

					listar_vendas(db);
				}
				else if (opc == 7) {

					menu = 2;
					opc = 0;
				}
				else if (opc == 8) {

					resetar_banco(db);
				}
				else if (opc == 9)
				{
					gerar_dados_teste(db);
				}

				break;
			}

			case 72:
			{
				if (opc > 0)
					opc--;
				else if (opc == 0) 
					if (menu == 1) opc = 9;
					else if (menu == 2) opc = 3;

				break;
			}

			case 80:
			{
				if (menu == 1) {
					if (opc < 9)
						opc++;
					else if (opc == 9)
						opc = 0;
				} else if (menu == 2) {
					if (opc < 3)
						opc++;
					else if (opc == 3)
						opc = 0;
				}
				break;
			}

			//case 75: printf("Left Arrow\n"); break;
			//case 77: printf("Right Arrow\n"); break;

			default: {
				// tecla inválida, não faz nada
			}
		}
	}
}

// Funções para imprimir o menu, destacando a opção selecionada com cursor (controlada pela variável global opc)
void printMenu()
{
	if (menu == 2) {
		printMenuRelatorios();
	}
	else if (menu == 1) {
		printMenuInicial();
	}
}

void printMenuInicial()
{
	system("cls");

	wcout << L"Escolha uma opção:\n(Use setas e 'Enter' para navegar)\n\n";

	(opc == 0) ? wcout << L">0. Sair\n" : wcout << L" 0. Sair\n";
	(opc == 1) ? wcout << L">1. Registrar Produto\n" : wcout << L" 1. Registrar Produto\n";
	(opc == 2) ? wcout << L">2. Listar Produtos\n" : wcout << L" 2. Listar Produtos\n";
	(opc == 3) ? wcout << L">3. Registrar Cliente\n" : wcout << L" 3. Registrar Cliente\n";
	(opc == 4) ? wcout << L">4. Listar Clientes\n" : wcout << L" 4. Listar Clientes\n";
	(opc == 5) ? wcout << L">5. Registrar Venda\n" : wcout << L" 5. Registrar Venda\n";
	(opc == 6) ? wcout << L">6. Listar Vendas\n" : wcout << L" 6. Listar Vendas\n";
	(opc == 7) ? wcout << L">7. Mostrar Relatórios\n" : wcout << L" 7. Mostrar Relatórios\n";
	(opc == 8) ? wcout << L">8. Resetar Banco\n" : wcout << L" 8. Resetar Banco\n";
	(opc == 9) ? wcout << L">9. Gerar Dados de Teste\n" : wcout << L" 9. Gerar Dados de Teste\n";
}

void printMenuRelatorios() 
{
	system("cls");

	wcout << L"Escolha uma opção:\n(Use setas e 'Enter' para navegar)\n\n";

	(opc == 0) ? wcout << L">0. Voltar ao Menu Principal\n" : wcout << L" 0. Voltar ao Menu Principal\n";
	(opc == 1) ? wcout << L">1. Relatório Diário\n" : wcout << L" 1. Relatório Diário\n";
	(opc == 2) ? wcout << L">2. Relatório Mensal\n" : wcout << L" 2. Relatório Mensal\n";
	(opc == 3) ? wcout << L">3. Relatório Anual\n" : wcout << L" 3. Relatório Anual\n";
}

// Conexão com o banco de dados. Criar e abrir o banco "empresa.db"
int banco_de_dados()
{
	int rc = sqlite3_open("empresa.db", &db);

	if (rc != SQLITE_OK)
	{
		wcout << L"Erro ao abrir banco\n";
		return rc;
	}

	// Habilitar chaves estrangeiras
	sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, 0);

	// Criar as tabelas clientes, produtos e vendas
	const char* sql =
		"CREATE TABLE IF NOT EXISTS clientes ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"ativo INTEGER DEFAULT 1,"
		"nome TEXT NOT NULL);"

		"CREATE TABLE IF NOT EXISTS produtos ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"ativo INTEGER DEFAULT 1,"
		"nome TEXT NOT NULL,"
		"preco REAL NOT NULL);"

		"CREATE TABLE IF NOT EXISTS vendas ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"ativo INTEGER DEFAULT 1,"
		"cliente_id INTEGER,"
		"produto_id INTEGER,"
		"dia INTEGER,"
		"mes INTEGER,"
		"ano INTEGER,"
		"quant_prod INTEGER,"
		"devolucao INTEGER,"
		"FOREIGN KEY(cliente_id) REFERENCES clientes(id),"
		"FOREIGN KEY(produto_id) REFERENCES produtos(id));";

	char* errMsg = 0;

	rc = sqlite3_exec(db, sql, 0, 0, &errMsg);

	if (rc != SQLITE_OK)
	{
		wcout
			<< L"Erro SQL: "
			<< sqlite3_errmsg(db)
			<< L"\n";
	}

	return rc;
}

// Função para resetar o banco de dados. Ela fecha a conexão atual, 
// remove o arquivo do banco e chama a função de configuração para criar 
// um novo banco vazio. Após resetar, volta para a opção "Sair" no menu.
void resetar_banco(sqlite3* db)
{
	sqlite3_close(db);

	remove("empresa.db");

	banco_de_dados();

	wcout << L"Banco resetado\n";

	readKey();
	opc = 0; // Voltar para a opção "Sair" após resetar o banco
}

// Funções de registro e listagem de produtos, clientes e vendas. 
// Elas interagem com o usuário para obter os dados necessários, 
// fazem as consultas para validar entradas (ex: verificar se cliente/produto existe) 
// e chamam as funções de inserção no banco de dados.
void registrar_produto()
{
	system("cls");

	wcout << L"- Registrar Produto\n\n";

	Produto p;

	wstring wn;

	wcout << L"Nome: ";
	getline(wcin, wn);

	p.nome = trim(utf8_from_wstring(wn));

	wcout << L"Preço: ";
	wcin >> p.preco;

	// limpar newline pendente
	wcin.ignore(numeric_limits<streamsize>::max(), L'\n');

	inserir_produto(db, p);

	wcout << L"\nProduto registrado\n";
	wcout << L"Pressione qualquer tecla...\n";

	readKey();
}

void registrar_cliente()
{
	system("cls");

	wcout << L"- Registrar Cliente\n\n";

	Cliente c;

	wstring wn;

	wcout << L"Nome: ";
	getline(wcin, wn);

	c.nome = trim(utf8_from_wstring(wn));

	inserir_cliente(db, c);

	wcout << L"\nCliente registrado\n";
	wcout << L"Pressione qualquer tecla...\n";

	readKey();
}

void registrar_venda()
{
	system("cls");

	wcout << L"- Registrar Venda\n\n";

	Venda v;

	bool encontrado = false;

	while (!encontrado)
	{
		wstring wtemp;
		string temp_nome;

		wcout << L"Nome do cliente: ";

		getline(wcin, wtemp);

		temp_nome = trim(utf8_from_wstring(wtemp));

		const char* sql =
			"SELECT id "
			"FROM clientes "
			"WHERE ativo = 1 AND TRIM(nome) = ? COLLATE NOCASE;";

		sqlite3_stmt* stmt;

		int rc = sqlite3_prepare_v2(
			db,
			sql,
			-1,
			&stmt,
			NULL
		);

		if (rc != SQLITE_OK)
		{
			wcout
				<< L"Erro SQL: "
				<< sqlite3_errmsg(db)
				<< L"\n";

			return;
		}

		sqlite3_bind_text(
			stmt,
			1,
			temp_nome.c_str(),
			-1,
			SQLITE_TRANSIENT
		);

		rc = sqlite3_step(stmt);

		if (rc == SQLITE_ROW)
		{
			v.cliente_id =
				sqlite3_column_int(stmt, 0);

			wcout << L"Cliente encontrado\n";

			encontrado = true;
		}
		else if (rc == SQLITE_DONE)
		{
			wcout << L"Cliente não encontrado\n";
		}
		else
		{
			wcout << L"Erro ao consultar cliente\n";
		}

		sqlite3_finalize(stmt);
	}

	encontrado = false;

	while (!encontrado)
	{
		wstring wtemp;
		string temp_nome;

		wcout << L"\nNome do Produto: ";

		getline(wcin, wtemp);

		temp_nome = trim(utf8_from_wstring(wtemp));

		const char* sql =
			"SELECT id "
			"FROM produtos "
			"WHERE ativo = 1 AND TRIM(nome) = ? COLLATE NOCASE;";

		sqlite3_stmt* stmt;

		int rc = sqlite3_prepare_v2(
			db,
			sql,
			-1,
			&stmt,
			NULL
		);

		if (rc != SQLITE_OK)
		{
			wcout
				<< L"Erro SQL: "
				<< sqlite3_errmsg(db)
				<< L"\n";

			return;
		}

		sqlite3_bind_text(
			stmt,
			1,
			temp_nome.c_str(),
			-1,
			SQLITE_TRANSIENT
		);

		rc = sqlite3_step(stmt);

		if (rc == SQLITE_ROW)
		{
			v.produto_id =
				sqlite3_column_int(stmt, 0);

			wcout << L"Produto encontrado\n";

			encontrado = true;
		}
		else if (rc == SQLITE_DONE)
		{
			wcout << L"Produto não encontrado\n";
		}
		else
		{
			wcout << L"Erro ao consultar produto\n";
		}

		sqlite3_finalize(stmt);
	}

	wcout << L"\nData (DD MM AAAA): ";
	wcin >> v.data.dia >> v.data.mes >> v.data.ano;

	wcout << L"\nQuantidade: ";
	wcin >> v.quant_prod;

	wcout << L"\nDevoluções: ";
	wcin >> v.devolucao;

	// limpar newline pendente
	wcin.ignore(numeric_limits<streamsize>::max(), L'\n');

	inserir_venda(db, v);

	wcout << L"\nVenda registrada\n";
	wcout << L"Pressione qualquer tecla...\n";

	readKey();
}

// Funções de listagem de produtos, clientes e vendas. 
// Elas fazem consultas no banco de dados para obter os 
// registros ativos e exibem as informações formatadas para o usuário.
void listar_produtos(sqlite3* db)
{
	system("cls");

	const char* sql =
		"SELECT id, nome, preco "
		"FROM produtos "
		"WHERE ativo = 1;";

	sqlite3_stmt* stmt;

	int rc = sqlite3_prepare_v2(
		db,
		sql,
		-1,
		&stmt,
		NULL
	);

	if (rc != SQLITE_OK)
	{
		wcout
			<< L"Erro SQL: "
			<< sqlite3_errmsg(db)
			<< L"\n";

		return;
	}

	wcout << L"=== PRODUTOS ===\n\n";

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		int id =
			sqlite3_column_int(stmt, 0);

		string nome_utf8 =
			reinterpret_cast<const char*>(
				sqlite3_column_text(stmt, 1)
				);

		wstring nome =
			wstring_from_utf8(nome_utf8);

		double preco =
			sqlite3_column_double(stmt, 2);

		wcout
			<< L"ID: " << id
			<< L" | Nome: " << nome
			<< L" | Preço: " << preco
			<< L"\n";
	}

	sqlite3_finalize(stmt);

	wcout << L"\nPressione qualquer tecla...\n";

	readKey();
}

void listar_clientes(sqlite3* db)
{
	system("cls");

	const char* sql =
		"SELECT id, nome "
		"FROM clientes "
		"WHERE ativo = 1;";

	sqlite3_stmt* stmt;

	int rc = sqlite3_prepare_v2(
		db,
		sql,
		-1,
		&stmt,
		NULL
	);

	if (rc != SQLITE_OK)
	{
		wcout
			<< L"Erro SQL: "
			<< sqlite3_errmsg(db)
			<< L"\n";

		return;
	}

	wcout << L"=== CLIENTES ===\n\n";

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		int id =
			sqlite3_column_int(stmt, 0);

		string nome_utf8 =
			reinterpret_cast<const char*>(
				sqlite3_column_text(stmt, 1)
				);

		wstring nome =
			wstring_from_utf8(nome_utf8);

		wcout
			<< L"ID: " << id
			<< L" | Nome: " << nome
			<< L"\n";
	}

	sqlite3_finalize(stmt);

	wcout << L"\nPressione qualquer tecla...\n";

	readKey();
}

void listar_vendas(sqlite3* db)
{
	int dia = 1;
	int mes = 1;
	int ano = 2026;

	while (true)
	{
		system("cls");

		wcout
			<< L"=== VENDAS "
			<< dia << L"/"
			<< mes << L"/"
			<< ano << L" ===\n\n";

		const char* sql =
			"SELECT "
			"v.id, "
			"c.nome, "
			"p.nome, "
			"p.preco, "
			"v.quant_prod, "
			"v.devolucao, "

			"CASE "
			"WHEN v.devolucao >= 2 "
			"THEN (p.preco * v.quant_prod) + 20 "
			"ELSE (p.preco * v.quant_prod) "
			"END AS total "

			"FROM vendas v "

			"INNER JOIN clientes c "
			"ON v.cliente_id = c.id "

			"INNER JOIN produtos p "
			"ON v.produto_id = p.id "

			"WHERE "
			"v.ativo = 1 "
			"AND v.dia = ? "
			"AND v.mes = ? "
			"AND v.ano = ?;";

		sqlite3_stmt* stmt;

		int rc =
			sqlite3_prepare_v2(
				db,
				sql,
				-1,
				&stmt,
				NULL
			);

		if (rc != SQLITE_OK)
		{
			wcout
				<< L"Erro SQL: "
				<< sqlite3_errmsg(db)
				<< L"\n";

			return;
		}

		sqlite3_bind_int(stmt, 1, dia);
		sqlite3_bind_int(stmt, 2, mes);
		sqlite3_bind_int(stmt, 3, ano);

		double total_dia = 0;
		int transcoes_dia = 0;
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			int id =
				sqlite3_column_int(stmt, 0);

			string cliente_utf8 =
				reinterpret_cast<const char*>(
					sqlite3_column_text(stmt, 1)
					);

			string produto_utf8 =
				reinterpret_cast<const char*>(
					sqlite3_column_text(stmt, 2)
					);

			wstring cliente =
				wstring_from_utf8(cliente_utf8);

			wstring produto =
				wstring_from_utf8(produto_utf8);

			double preco =
				sqlite3_column_double(stmt, 3);

			int quant =
				sqlite3_column_int(stmt, 4);

			int devolucao =
				sqlite3_column_int(stmt, 5);

			double total =
				sqlite3_column_double(stmt, 6);

			wcout
				<< L"ID: " << id
				<< L" | Cliente: " << cliente
				<< L" | Produto: " << produto
				<< L" | Quantidade: " << quant
				<< L" | Devoluções: " << devolucao
				<< L" | Preço Unidade: R$ " << preco
				<< L" | Total: R$ " << total
				<< L"\n";

			total_dia += total;
			transcoes_dia++;
		}

		sqlite3_finalize(stmt);

		wcout
			<< L"\nTOTAL DO DIA: R$ " << total_dia
			<< L" | TOTAL DE VENDAS: " << transcoes_dia
			<< L"\n";

		wcout
			<< L"\n----------------------------------------------------------------------------------------------------"
			<< L"\n← Dia anterior | → Próximo dia | 'Enter' Definir Data | ESC Sair\n";

		int key = readKey();

		if (key == 27)
		{
			break;
		}
		else if (key == 77)
		{
			dia++;

			int maxDias = 31;

			if (
				mes == 4 ||
				mes == 6 ||
				mes == 9 ||
				mes == 11
				)
			{
				maxDias = 30;
			}

			if (mes == 2)
			{
				maxDias = 28;
			}

			if (dia > maxDias)
			{
				dia = 1;
				mes++;

				if (mes > 12)
				{
					mes = 1;
					ano++;
				}
			}
		}
		else if (key == 75)
		{
			dia--;

			if (dia < 1)
			{
				mes--;

				if (mes < 1)
				{
					mes = 12;
					ano--;
				}

				if (
					mes == 4 ||
					mes == 6 ||
					mes == 9 ||
					mes == 11
					)
				{
					dia = 30;
				}
				else if (mes == 2)
				{
					dia = 28;
				}
				else
				{
					dia = 31;
				}
			}
		}
		else if (key == 13) 
		{
			wcout << L"\nData (DD MM AAAA): ";

			wcin >> dia >> mes >> ano;

			wcin.ignore(
				numeric_limits<streamsize>::max(),
				L'\n'
			);
		}
	}
}

// Funções de inserção de produtos, clientes e vendas no banco de dados.
void inserir_produto(sqlite3* db, Produto produto)
{
	const char* sql =
		"INSERT INTO produtos "
		"(nome, preco, ativo) "
		"VALUES (?, ?, 1);";

	sqlite3_stmt* stmt;

	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

	if (rc != SQLITE_OK)
	{
		wcout
			<< L"Erro SQL: "
			<< sqlite3_errmsg(db)
			<< L"\n";

		return;
	}

	sqlite3_bind_text(stmt, 1, produto.nome.c_str(), -1, SQLITE_TRANSIENT);

	sqlite3_bind_double(stmt, 2, produto.preco);

	rc = sqlite3_step(stmt);

	if (rc != SQLITE_DONE)
	{
		wcout << L"Erro ao inserir produto\n";
	}

	sqlite3_finalize(stmt);
}

void inserir_cliente(sqlite3* db, Cliente cliente)
{
	const char* sql =
		"INSERT INTO clientes "
		"(nome, ativo) "
		"VALUES (?, 1);";

	sqlite3_stmt* stmt;

	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

	if (rc != SQLITE_OK)
	{
		wcout
			<< L"Erro SQL: "
			<< sqlite3_errmsg(db)
			<< L"\n";

		return;
	}

	sqlite3_bind_text(stmt, 1, cliente.nome.c_str(), -1, SQLITE_TRANSIENT);

	rc = sqlite3_step(stmt);

	if (rc != SQLITE_DONE)
	{
		wcout << L"Erro ao inserir cliente\n";
	}

	sqlite3_finalize(stmt);
}

void inserir_venda(sqlite3* db, Venda venda)
{
	const char* sql =
		"INSERT INTO vendas "
		"(cliente_id, produto_id, dia, mes, ano, quant_prod, devolucao, ativo) "
		"VALUES (?, ?, ?, ?, ?, ?, ?, 1);";

	sqlite3_stmt* stmt;

	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

	if (rc != SQLITE_OK)
	{
		wcout
			<< L"Erro SQL: "
			<< sqlite3_errmsg(db)
			<< L"\n";

		return;
	}

	sqlite3_bind_int(stmt, 1, venda.cliente_id);
	sqlite3_bind_int(stmt, 2, venda.produto_id);
	sqlite3_bind_int(stmt, 3, venda.data.dia);
	sqlite3_bind_int(stmt, 4, venda.data.mes);
	sqlite3_bind_int(stmt, 5, venda.data.ano);
	sqlite3_bind_int(stmt, 6, venda.quant_prod);
	sqlite3_bind_int(stmt, 7, venda.devolucao);

	rc = sqlite3_step(stmt);

	if (rc != SQLITE_DONE)
	{
		wcout << L"Erro ao inserir venda\n";
	}

	sqlite3_finalize(stmt);
}

// Funcões para gerar relatórios diário, mensal e anual. Elas fazem consultas no banco de dados para obter os dados agregados
void relatorio_diario(sqlite3* db)
{
	listar_vendas(db);
}

void relatorio_mensal(sqlite3* db)
{
	int mes = 1;
	int ano = 2026;

	while (true)
	{
		system("cls");

		wcout
			<< L"=== VENDAS MÊS "
			<< mes << L"/"
			<< ano << L" ===\n\n";

		const char* sql =
			"SELECT "

			"SUM( "

			"CASE "
			"WHEN v.devolucao >= 2 "
			"THEN (p.preco * v.quant_prod) + 20 "
			"ELSE (p.preco * v.quant_prod) "
			"END "

			") AS total_mes, "

			"COUNT(*) AS total_vendas "

			"FROM vendas v "

			"INNER JOIN produtos p "
			"ON v.produto_id = p.id "

			"WHERE "
			"v.ativo = 1 "
			"AND v.mes = ? "
			"AND v.ano = ?;";

		sqlite3_stmt* stmt;

		int rc =
			sqlite3_prepare_v2(
				db,
				sql,
				-1,
				&stmt,
				NULL
			);

		if (rc != SQLITE_OK)
		{
			wcout
				<< L"Erro SQL: "
				<< sqlite3_errmsg(db)
				<< L"\n";

			return;
		}

		sqlite3_bind_int(stmt, 1, mes);
		sqlite3_bind_int(stmt, 2, ano);

		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			double total_mes =
				sqlite3_column_double(stmt, 0);

			int total_vendas =
				sqlite3_column_int(stmt, 1);

			wcout
				<< L"TOTAL VENDIDO: R$ "
				<< total_mes
				<< L"\n\n";

			wcout
				<< L"QUANTIDADE DE VENDAS: "
				<< total_vendas
				<< L"\n";
		}
		else
		{
			wcout
				<< L"Nenhum dado encontrado.\n";
		}

		sqlite3_finalize(stmt);

		wcout
			<< L"\n----------------------------------------------------------------------------------------------------\n"

			<< L"← Mês anterior | → Próximo mês | 'Enter' Definir Data | ESC Sair\n";

		int key = readKey();

		if (key == 27)
		{
			break;
		}

		else if (key == 77)
		{
			mes++;

			if (mes > 12)
			{
				mes = 1;
				ano++;
			}
		}

		else if (key == 75)
		{
			mes--;

			if (mes < 1)
			{
				mes = 12;
				ano--;
			}
		}

		else if (key == 13)
		{
			wcout << L"\nData (MM AAAA): ";

			wcin >> mes >> ano;

			wcin.ignore(
				numeric_limits<streamsize>::max(),
				L'\n'
			);
		}
	}
}

void relatorio_anual(sqlite3* db)
{
	int ano = 2026;

	while (true)
	{
		system("cls");

		wcout
			<< L"=== RELATÓRIO ANUAL ===\n\n"

			<< L"Ano: "
			<< ano
			<< L"\n\n";

		const char* sql =

			"SELECT "
			"v.mes, "

			"SUM( "
			"CASE "
			"WHEN v.devolucao >= 2 "
			"THEN (p.preco * v.quant_prod) + 20 "
			"ELSE (p.preco * v.quant_prod) "
			"END "
			") AS total_mes "

			"FROM vendas v "
			"INNER JOIN produtos p "
			"ON v.produto_id = p.id "
			"WHERE "
			"v.ativo = 1 "
			"AND v.ano = ? "

			"GROUP BY v.mes "

			"ORDER BY total_mes DESC;";

		sqlite3_stmt* stmt;

		int rc =
			sqlite3_prepare_v2(
				db,
				sql,
				-1,
				&stmt,
				NULL
			);

		if (rc != SQLITE_OK)
		{
			wcout
				<< L"Erro SQL: "
				<< sqlite3_errmsg(db)
				<< L"\n";

			return;
		}

		sqlite3_bind_int(stmt, 1, ano);

		double total_ano = 0;
		bool encontrou = false;

		wcout
			<< L"MESES EM ORDEM DE FATURAMENTO\n\n";

		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			encontrou = true;

			int mes =
				sqlite3_column_int(stmt, 0);

			double total_mes =
				sqlite3_column_double(stmt, 1);

			total_ano += total_mes;

			wcout
				<< L"Mês "
				<< mes
				<< L" | Total: R$ "
				<< total_mes
				<< L"\n";
		}

		if (!encontrou)
		{
			wcout
				<< L"Nenhuma venda encontrada.\n";
		}

		wcout
			<< L"\nTOTAL ANUAL: R$ "
			<< total_ano
			<< L"\n";

		sqlite3_finalize(stmt);

		wcout
			<< L"\n----------------------------------------------------------------------------------------------------\n"

			<< L"← Ano anterior | → Próximo ano | 'Enter' Definir Ano | ESC Sair\n";

		int key = readKey();

		if (key == 27)
		{
			break;
		}

		else if (key == 77)
		{
			ano++;
		}

		else if (key == 75)
		{
			ano--;
		}

		else if (key == 13)
		{
			wcout << L"\nAno: ";

			wcin >> ano;

			wcin.ignore(
				numeric_limits<streamsize>::max(),
				L'\n'
			);
		}
	}
}

// Função para gerar dados de teste no banco de dados. Ela cria uma lista de clientes e produtos pré-definidos,
// e depois gera 50 vendas aleatórias para cada dia do ano, associando clientes e produtos de forma randômica.
void gerar_dados_teste(sqlite3* db)
{
	sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, 0);

	srand((unsigned)time(NULL));

	vector<wstring> clientes = {
		L"Maria   ",
		L"João    ",
		L"Pedro   ",
		L"Ana     ",
		L"Carlos  ",
		L"Fernanda",
		L"Lucas   ",
		L"Juliana ",
		L"Rafael  ",
		L"Patrícia"
	};

	vector<wstring> produtos = {
		L"Notebook  ",
		L"Mouse     ",
		L"Teclado   ",
		L"Monitor   ",
		L"Headset   ",
		L"Celular   ",
		L"Impressora",
		L"Webcam    ",
		L"SSD       ",
		L"Microfone "
	};

	vector<float> precos = {
		3500.0f,
		80.0f,
		150.0f,
		900.0f,
		250.0f,
		2200.0f,
		700.0f,
		120.0f,
		400.0f,
		300.0f
	};

	// CLIENTES

	for (int i = 0; i < 10; i++)
	{
		Cliente c;

		c.nome = utf8_from_wstring(clientes[i]);

		inserir_cliente(db, c);
	}

	// PRODUTOS

	for (int i = 0; i < 10; i++)
	{
		Produto p;

		p.nome = utf8_from_wstring(produtos[i]);
		p.preco = precos[i];

		inserir_produto(db, p);
	}

	// VENDAS

	for (int mes = 1; mes <= 12; mes++)
	{
		int dias_mes = 31;

		if (mes == 2)
			dias_mes = 28;

		else if (
			mes == 4 ||
			mes == 6 ||
			mes == 9 ||
			mes == 11
			)
			dias_mes = 30;

		for (int dia = 1; dia <= dias_mes; dia++)
		{
			for (int i = 0; i < 50; i++)
			{
				Venda v;

				v.cliente_id = (rand() % 10) + 1;

				v.produto_id = (rand() % 10) + 1;

				v.data.dia = dia;
				v.data.mes = mes;
				v.data.ano = 2026;

				v.quant_prod =
					(rand() % 5) + 1;

				v.devolucao =
					(rand() % 3);

				inserir_venda(db, v);
			}
		}
	}

	wcout << L"Dados gerados com sucesso\n";

	sqlite3_exec(db, "COMMIT;", 0, 0, 0);

	readKey();
}

// Função auxiliar para ler tecla e tratar teclas estendidas (setas)
int readKey()
{
	int ch = _getch();

	if (ch == 0 || ch == 224)
	{
		ch = _getch();
	}

	return ch;
}

// Função auxiliar para conversão de tipos de string entre UTF-8 e UTF-16 (wstring) usando as APIs do Windows
string utf8_from_wstring(const wstring& w)
{
	if (w.empty())
		return string();

	int size_needed =
		WideCharToMultiByte(
			CP_UTF8,
			0,
			w.c_str(),
			-1,
			NULL,
			0,
			NULL,
			NULL
		);

	string s(size_needed - 1, '\0');

	WideCharToMultiByte(
		CP_UTF8,
		0,
		w.c_str(),
		-1,
		&s[0],
		size_needed,
		NULL,
		NULL
	);

	return s;
}

// Função auxiliar para conversão de tipos de string entre UTF-8 e UTF-16 (wstring) usando as APIs do Windows
wstring wstring_from_utf8(const string& str)
{
	if (str.empty())
		return wstring();

	int size_needed =
		MultiByteToWideChar(
			CP_UTF8,
			0,
			str.c_str(),
			-1,
			NULL,
			0
		);

	wstring w(size_needed - 1, 0);

	MultiByteToWideChar(
		CP_UTF8,
		0,
		str.c_str(),
		-1,
		&w[0],
		size_needed
	);

	return w;
}

// Função auxiliar para remover espaços em branco no início e fim de uma string
string trim(const string& str)
{
	size_t inicio =
		str.find_first_not_of(" \t\n\r");

	if (inicio == string::npos)
		return "";

	size_t fim =
		str.find_last_not_of(" \t\n\r");

	return str.substr(
		inicio,
		fim - inicio + 1
	);
}