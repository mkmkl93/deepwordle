import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
import matplotlib
import bisect
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
    ax = sns.heatmap(differences[450:651, 450:651], cbar_kws={'label': 'number of differentiating words'})
    ax.set(xticks=np.arange(0, 201, 50), xticklabels=np.arange(450, 651, 50))
    ax.set(yticks=np.arange(0, 201, 50), yticklabels=np.arange(450, 651, 50))
    plt.savefig('zoom300.png')
    plt.clf()

    ####

    ax = sns.heatmap(differences[450:650, 450:650], cbar_kws={'label': 'number of differentiating words'})
    ax.set(xticks=np.arange(0, 201, 50), xticklabels=np.arange(450, 651, 50))
    ax.set(yticks=np.arange(0, 201, 50), yticklabels=np.arange(450, 651, 50))
    plt.axvline(62, 0, 200, c="blue")
    plt.text(50, -5, "daddy")
    plt.axhline(62, 0, 200)
    plt.text(-25, 63, "daddy")

    plt.axvline(173, 0, 300, c="red")
    plt.text(161, -5, "eager")
    plt.axhline(173, 0, 300, c="red")
    plt.text(-25, 174, "eager")
    plt.savefig('zoom300_labeled.png')
    plt.clf()

    ####

    sns.set(rc={'figure.figsize': (25, 20)})
    sns.heatmap(differences, xticklabels=200, yticklabels=200,  cbar_kws={'label': 'number of differentiating words'})
    plt.savefig('all.png')
    plt.clf()


def distribution():
    arr = np.reshape(differences, -1)
    arr = np.sort(arr)
    arr = arr[arr > 0]
    print(arr[:10])

    # sns.displot(arr, bins=word_list_len)
    # plt.savefig('histogram.png', dpi=2000)


def get_differenting_words(id1, id2):
    list = []

    word1 = id1
    word2 = id2
    for g in range(word_list_len):
        if response(word_list[g], word_list[word1]) != response(word_list[g], word_list[word2]):
            list.append(word_list[g])
            # print('"' + word_list[g] + '", ', end='')

    return list

def get_worst(threshold):
    cands = []

    for i in range(word_list_len):
        for j in range(i + 1, word_list_len):
            if differences[i][j] <= threshold and differences[i][j] != 0:
                cands.append((i, j))
                print(i, j, word_list[i], word_list[j], differences[i][j])

    return cands


def solve_pairs(pairs):
    res = []
    # print(pairs)

    for i in range(len(pairs)):
        (w1, w2) = pairs[i]
        id1 = bisect.bisect_left(word_list, w1)
        id2 = bisect.bisect_left(word_list, w2)

        diff = get_differenting_words(id1, id2)

        if i == 0:
            res = diff
        else:
            tmp = [v for v in diff if v in res]
            res = tmp

    return res

if __name__ == "__main__":
    # calculate_differences()
    differences = np.load('differences.npy')
    # save_heatmap()
    # distribution()

    # cands = get_worst(220)

    unsolved = [("bobby", "booby"), ("paper", "parer"), ("truss", "trust"), ("crock", "crook")]

    print(solve_pairs([("catch", "hatch"), ("ample", "maple"), ("eagle", "legal"), ("droll", "drool"), ("abode",
                                                                                                     "adobe")]))
    print(solve_pairs([("bobby", "booby"), ("dowry", "rowdy"), ("truss", "trust"), ("piper", "riper"),
                       ("crock", "crook")]))
    print(solve_pairs([("bobby", "booby"), ("rogue", "rouge"), ("truss", "trust"), ("below", "elbow"), ("crock", "crook")]))
    print(solve_pairs([("catch", "hatch"), ("ample", "maple"), ("eagle", "legal"), ("droll", "drool"), ("abode", "adobe")]))
    print(solve_pairs([("catch", "hatch"), ("droll", "drool"), ("ample", "maple"), ("eagle", "legal"), ("abode", "adobe")]))
    print(solve_pairs([("skate", "stake"), ("chili", "chill"), ("pasty", "patsy"), ("snoop", "spoon"), ("abode",
                                                                                                          "adobe")]))
    # print(solve_pairs([("abode", "adobe")]))
    #
    # print(solve_pairs(unsolved[1:]))
    # print(solve_pairs(unsolved[:1] + unsolved[2:]))
    # print(solve_pairs(unsolved[:2] + unsolved[3:]))
    # print(solve_pairs(unsolved[:3]))
    # print(solve_pairs(unsolved))