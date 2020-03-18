from sys import argv

iptFile = open(argv[1], mode='r', encoding='big5-hkscs')

lines = iptFile.read()
mapping = dict()
for line in lines.split("\n"):
	if len(line) != 0:
		word, zhus = line.split(" ")
		zhu = zhus.split("/")
		zhuHead = zhu[0][0]

		for i in zhuHead:
			if i not in mapping:
				mapping[i] = [word]
			else:
				if word not in mapping[i]:
					mapping[i].append(word)

zhulist = ['ㄅ','ㄆ','ㄇ','ㄈ','ㄉ','ㄊ','ㄋ','ㄌ','ㄍ','ㄎ','ㄏ','ㄐ','ㄑ','ㄒ','ㄓ','ㄔ','ㄕ','ㄖ','ㄗ','ㄘ','ㄙ','ㄧ','ㄨ','ㄩ','ㄚ','ㄛ','ㄜ','ㄝ','ㄞ','ㄟ','ㄠ','ㄡ','ㄢ','ㄣ','ㄤ','ㄥ','ㄦ']
optFile = open("ZhuYin-Big5.map", mode='w', encoding='big5-hkscs')

for i in zhulist:
	if i in mapping:
		mapWord = mapping[i]
		optFile.write(i + "\t")
		for j in mapWord:
			optFile.write(j + " ")
		optFile.write("\n")
		for j in mapWord:
			optFile.write(j + "\t" + j + "\n")
optFile.close()
	


