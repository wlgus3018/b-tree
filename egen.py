import random


def gen():
        num = 20
    	ch_list = [chr(i) for i in range(ord('a'), ord('z') + 1)] 
	length = 1
        k = random.sample(range(1,30),20)
	with open("data/rand-data.txt", "w") as F:
		for i in range(num):
                        key = k[i]
			F.write(str(key))
			F.write(" " + "".join([random.choice(ch_list) for i in range(length)]) + "\n")

def main():
        gen()

main()