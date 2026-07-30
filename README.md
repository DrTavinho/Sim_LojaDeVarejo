# Sim_LojaDeVarejo

Um sistema de controle de vendas com relatórios baseado em banco de dados SQL Lite.

---

# Requisitos do programa

- registrar/listar produtos
  - valor (preço) do produto
  - nome do produto
- registrar/listar clientes
  - nome do cliente
- registrar/listar vendas
  - anexar um ID de cliente
  - anexar um ID de produto 
  - quantidade adquirida do produto
  - quantidade de devoluções realizadas
  - data de realização da venda
  - apresentar valor total e unitário da venda

# Regras de negócio:

- caso algum item do pedido seja devolvido pela segunda vez, deverá ser cobrada uma taxa fixa de R$ 20,00, referente às despesas da transportadora.

---

# Relatórios obrigatórios

O programa gera os seguintes relatórios:

1) Relatório diário

Uma listagem contendo:

- os pedidos realizados no dia
- os respectivos valores
- o valor total das vendas diárias

2) Relatório mensal

Uma listagem mensal contendo:

- o total vendido em cada mês
- o valor total consolidado das vendas mensais

3) Relatório anual

O programa deverá:

- calcular o total das vendas realizadas ao longo do ano
- apresentar os meses em ordem decrescente de faturamento, iniciando pelo mês com maior valor vendido até o menor

---

# Restrição

A loja realiza 50 vendas por dia, e essa quantidade deverá ser considerada no desenvolvimento do programa.






