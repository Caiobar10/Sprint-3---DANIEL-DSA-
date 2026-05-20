# BLUA — Sistema de Gerenciamento de Fila de Atendimento

**Care Plus | Plataforma de Teleconsulta Remota**

Sistema em C que gerencia o fluxo de atendimentos remotos da plataforma Blua. As estruturas de Fila (FIFO) e Pilha (LIFO) foram implementadas **do zero com listas encadeadas**, sem uso de nenhuma biblioteca pronta de fila ou pilha — conforme requisito do projeto.

## Vídeo de demonstração
[Assistir no YouTube](https://youtu.be/B-Un_7V8WRc)
---

## Estruturas de Dados Implementadas

| Estrutura | Tipo | Papel no sistema |
|---|---|---|
| `Fila` — lista encadeada simples | FIFO | Fila de espera normal e fila de emergências |
| `Pilha` — lista encadeada simples | LIFO | Histórico auditável das últimas N consultas; desfazer/cancelar ações administrativas |

Cada estrutura implementa as operações obrigatórias:

| Operação | Fila | Pilha | O quê faz |
|---|---|---|---|
| inserir | `enqueue()` | `push()` | Adiciona um elemento |
| remover | `dequeue()` | `pop()` | Remove e retorna o elemento de saída |
| consultar | `peek_fila()` | `peek_pilha()` | Lê sem remover |
| verificar vazio | `fila_vazia()` | `pilha_vazia()` | Retorna 1 se vazia |

---

## Como Compilar e Executar

**Pré-requisito:** GCC instalado (`gcc --version` para verificar).

```bash
# Compilar
gcc -Wall -o blua blua.c

# Executar
./blua
```

O programa abre um menu interativo. Use as opções 7, 8 e 9 para rodar os cenários demonstrativos diretamente.

---

## Cenários Demonstrados

**Cenário 1 — Fluxo normal de atendimento**
Três pacientes chegam e são atendidos na ordem de chegada (FIFO). Cada consulta finalizada é registrada no topo da pilha de histórico (LIFO). Demonstra: `enqueue`, `dequeue`, `push`, exibição das estruturas.

**Cenário 2 — Emergência com fila prioritária**
Três pacientes normais estão na fila quando um alerta crítico chega. O sistema esvazia a fila de emergência antes de retomar a fila normal, simulando priorização real de triagem. Demonstra: duas filas independentes, lógica de prioridade, `peek` para visualização sem consumo.

**Cenário 3 — Cancelamento e auditoria administrativa**
Cinco pacientes são atendidos, mas o histórico tem capacidade máxima de 3 (política de auditoria). Os registros mais antigos são descartados automaticamente ao atingir o limite. Um administrador usa `pop` para cancelar/desfazer o último registro por erro de reatribuição, e `peek` para confirmar o novo topo sem alterar o histórico. Demonstra: pilha como mecanismo de desfazer (undo), limite LIFO, auditoria.

---

## Análise de Complexidade

| Operação | Complexidade | Justificativa |
|---|---|---|
| `enqueue` | O(1) | Inserção direto no ponteiro `fim` |
| `dequeue` | O(1) | Remoção direto no ponteiro `inicio` |
| `push` | O(1) | Inserção no `topo` |
| `pop` | O(1) | Remoção no `topo` |
| `peek` (fila e pilha) | O(1) | Leitura direta do ponteiro |
| `fila_vazia` / `pilha_vazia` | O(1) | Checagem de ponteiro NULL |
| Exibir estrutura inteira | O(n) | Percorre todos os nós |
| Remoção do fundo da pilha (limite N) | O(n) | Trade-off consciente: mantém todas as outras operações O(1) sem estrutura auxiliar |

> A remoção do elemento mais antigo da pilha ao atingir o limite é a única operação O(n). Essa escolha preserva O(1) em todas as operações do caminho crítico (atender paciente, registrar consulta, desfazer ação) e ocorre apenas ao escrever no histórico quando ele está cheio — situação controlada e previsível.

---

## Estrutura do Projeto

```
blua.c      — código-fonte principal com menu interativo
README.md   — este arquivo
```
