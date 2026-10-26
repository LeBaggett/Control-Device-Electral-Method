# Reset completo do termopar

Atualização de código aplicada; o reset dos ajustes da placa será executado
quando você confirmar no menu, depois de gravar o novo firmware.

Terminate Debug > Refresh > Clean > Build > gravar.
Depois: CAL TERMOPAR > RESET TOTAL TC > CONFIRMAR RESET.
Aguarde TERMOPAR RESETADO. Não precisa recompilar depois de usar o menu.

Apaga a sessão de calibração em 3/4 pontos, as referências coletadas e o candidato;
restaura ganho=1, offset=0, filtro=0,25 e número de pontos salvos=0. Reseta o
histórico do filtro de temperatura e atualiza a indicação na próxima leitura.
TCAL.TXT e TCAL.BAK recebem os padrões. Não apaga CSVs de ensaios.
Em erro no SD, não confirma o reset; confira o cartão e repita a operação.

Testes de software completos e compilação/link ARM passaram. Foi verificado
reset em RAM, recarga dos padrões, erro de escrita e fallback do backup após
corrupção simulada do arquivo principal. Sem validação física pelo assistente.
