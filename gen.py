import random


def gen():
        num = 50000000
        num2 = 50000001
    	ch_list = [chr(i) for i in range(ord('a'), ord('z') + 1)] 
	length = 1
        k = random.sample(range(1,99999999),60000000)
        i = 1
	with open("data/small-data.txt", "w") as F:
		for i in range(num):
			#key = random.randint(3000001, 99999999)
                        key = k[i]
			F.write(str(key))
			F.write(" " + "".join([random.choice(ch_list) for i in range(length)]) + "\n")
        with open("data/rand-data.txt", "w") as F:
		for i in range(5000000):
			#key = random.randint(3000001, 99999999)
                        key = k[num2+i]
			F.write(str(key))
			F.write(" " + "".join([random.choice(ch_list) for i in range(length)]) + "\n")



def main():
        gen()

main()
