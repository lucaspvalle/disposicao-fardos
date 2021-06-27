// header.h: arquivo de inclusão para arquivos de inclusão padrão do sistema,
// ou arquivos de inclusão específicos a um projeto

#pragma once
#include "headers/targetver.h"
#define WIN32_LEAN_AND_MEAN  // Excluir itens raramente utilizados dos cabeçalhos do Windows

// Arquivos de Cabeçalho do Windows
#include <windows.h>

// Arquivos de Cabeçalho C RunTime
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <ShObjIdl.h>
#include <gdiplus.h>

// Arquivos de Projeto
#include <random>
#include <iostream>
#include <string>
#include "headers/ga.h"