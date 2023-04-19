import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
from tqdm import tqdm

words_filename = '../wordle solutions sorted.txt'

word_list = []
with open(words_filename) as f:
    for line in f.readlines():
        word_list.append(line.strip())
# word_list = word_list[:500]
word_list_len = len(word_list)
print(word_list_len)
differences = np.zeros((word_list_len, word_list_len), dtype=np.uint)


def response(guess, solution):
    guess = list(guess)
    solution = list(solution)

    assert (len(guess) == len(solution))

    n = len(guess)
    res = ['0'] * n

    for i in range(n):
        if guess[i] == solution[i]:
            res[i] = '2'
            guess[i] = ''
            solution[i] = ''

    for i in range(n):
        if guess[i] == '':
            continue

        if guess[i] in solution:
            index = solution.index(guess[i])

            res[i] = '1'
            guess[i] = ''
            solution[index] = ''

    res = int("".join(res))

    return res


def calculate_differences():
    clues = np.zeros((word_list_len, word_list_len), dtype=np.uint)
    for idx, x in enumerate(tqdm(word_list)):
        for idy, y in enumerate(word_list):
            clues[idx][idy] = response(x, y)

    for g in tqdm(range(word_list_len)):
        for x in range(word_list_len):
            for y in range(word_list_len):
                if clues[g][x] != clues[g][y]:
                    differences[x][y] += 1

    np.save('differences.npy', differences)


def save_heatmap():
    sns.set(rc={'figure.figsize': (25, 20)})
    sns.heatmap(differences)

    plt.savefig('image.png', dpi=1000)


def distribution():
    arr = np.reshape(differences, -1)
    arr = np.sort(arr)
    arr = arr[arr > 0]
    print(arr[:10])

    # sns.displot(arr, bins=word_list_len)
    # plt.savefig('histogram.png', dpi=2000)


if __name__ == "__main__":
    # calculate_differences()
    differences = np.load('differences.npy')
    # save_heatmap()
    # distribution()

    cands = []

    for i in range(word_list_len):
        for j in range(i + 1, word_list_len):
            if differences[i][j] <= 150 and differences[i][j] != 0:
                cands.append((i, j))
                print(i, j, word_list[i], word_list[j])

    list1 = []

    word1 = 8
    word2 = 23
    for g in range(word_list_len):
        if response(word_list[g], word_list[word1]) != response(word_list[g], word_list[word2]):
            list1.append(word_list[g])
            print('"' + word_list[g] + '", ', end='')
            # print(word_list[g], response(word_list[g], word_list[word1]), response(word_list[g], word_list[word2]))

    # list2 = []
    # word1 = 242
    # word2 = 247
    # for g in range(word_list_len):
    #     if response(word_list[g], word_list[word1]) != response(word_list[g], word_list[word2]):
    #         list2.append(word_list[g])
    #         print('"' + word_list[g] + '", ', end='')

    # lst3 = [value for value in list1 if value in list2]
    #
    # res = []

    # for (x1, y1) in cands:
    #     for (x2, y2) in cands:
    #         if x1 == x2 and y1 == y2:
    #             continue
    #
    #         if x1 > x2:
    #             continue
    #
    #         list1 = []
    #
    #         for g in range(word_list_len):
    #             if response(word_list[g], word_list[x1]) != response(word_list[g], word_list[y1]):
    #                 list1.append(word_list[g])
    #
    #         list2 = []
    #
    #         for g in range(word_list_len):
    #             if response(word_list[g], word_list[x2]) != response(word_list[g], word_list[y2]):
    #                 list2.append(word_list[g])
    #
    #         lst3 = [value for value in list1 if value in list2]
    #
    #         res.append(len(lst3))
    #         # if len(lst3) <= 2:
    #         #     print(lst3)
    #         print(len(lst3), "\t\t", x1, y1, x2, y2, lst3)

    # res.sort()
    # print(res[:10])

    # print(len(lst3), lst3)
