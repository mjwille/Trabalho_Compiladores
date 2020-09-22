import unittest
from os import system, chdir, popen
from time import sleep

etapa = 'etapa3'
astOut1 = 'out1.txt'
astOut2 = 'out2.txt'

def compareAST(source):
	# executa o compilador sobre os arquivos
	system(f'./{etapa} tests/{source}  tests/{astOut1} > /dev/null')
	system(f'./{etapa} tests/{astOut1} tests/{astOut2} > /dev/null')
	# faz a comparação dos 2 arquivos de saída para ver se são iguais
	stream = popen(f'diff tests/{astOut1} tests/{astOut2}')
	output = stream.read()
	stream.close()
	# se iguais, diff não retorna nada
	if len(output) == 0:
		return True
	else:
		return False


class TestLanguage(unittest.TestCase):

	def testComparison(self):
		# muda o diretório para o anterior ao dos testes, pra fazer make e make clean
		chdir('..')
		# faz o build do compilador
		system('make')
		self.assertTrue(compareAST('test0.txt'))
		self.assertTrue(compareAST('test1.txt'))
		self.assertTrue(compareAST('test2.txt'))
		self.assertTrue(compareAST('test3.txt'))
		self.assertTrue(compareAST('test4.txt'))
		self.assertTrue(compareAST('test5.txt'))
		self.assertTrue(compareAST('test6.txt'))
		# remove os arquivos de saída par acomparação das AST
		system(f'rm tests/{astOut1} tests/{astOut2}')
		# limpa o build do compilador
		system('make clean')


if __name__ == "__main__":
	unittest.main()