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

def save_letters_count1():
    count = [
        [0, 0, 0, 4, 37, 62],
        [0, 0, 10, 111, 306, 0],
        [1, 12, 163, 602, 0, 0],
        [11, 111, 577, 0, 0, 0],
        [48, 209, 0, 0, 0, 0],
        [51, 0, 0, 0, 0, 0]
     ]

    ax = sns.heatmap(count, annot=True, fmt=".0f", cbar_kws={'label': 'size of corresponding group'}, cmap="crest")
    ax.set(xlabel="number of yellow letters", ylabel="number of green letters")
    ax.tick_params(top=True, labeltop=True, bottom=False, labelbottom=False)
    plt.savefig('count1.png')
    plt.clf()

def save_letters_count2():
    count = [
        [0, 0, 0, 4, 37, 47],
        [0, 0, 10, 96, 225, 0],
        [1, 11, 139, 349, 0, 0],
        [9, 92, 355, 0, 0, 0],
        [99, 0, 0, 0, 0, 0],
        [841, 0, 0, 0, 0, 0],
     ]

    ax = sns.heatmap(count, annot=True, fmt=".0f", cbar_kws={'label': 'size of corresponding group'}, cmap="crest")
    ax.set(xlabel="number of yellow letters", ylabel="number of green letters")
    ax.tick_params(top=True, labeltop=True, bottom=False, labelbottom=False)
    plt.savefig('count2.png')
    plt.clf()

def save_letters_count_robot():
    count = [
        [0, 0, 0, 4, 37, 47],
        [0, 0, 10, 96, 225, 0],
        [1, 11, 139, 349, 0, 0],
        [9, 92, 355, 0, 0, 0],
        [99, 0, 0, 0, 0, 0],
        [841, 0, 0, 0, 0, 0],
     ]

    ax = sns.heatmap(count, annot=True, fmt=".0f", cbar_kws={'label': 'final guess robot'}, cmap="crest")
    ax.set(xlabel="number of yellow letters", ylabel="number of green letters")
    ax.tick_params(top=True, labeltop=True, bottom=False, labelbottom=False)
    plt.savefig('count_robot.png')
    plt.clf()


def save_letters_count_roost():
    count = [
        [0, 0, 0, 3, 35, 52],
        [0, 0, 11, 93, 232, 0],
        [1, 9, 125, 318, 0, 0],
        [8, 97, 370, 0, 0, 0],
        [112, 0, 0, 0, 0, 0],
        [849, 0, 0, 0, 0, 0],
    ]

    ax = sns.heatmap(count, annot=True, fmt=".0f", cbar_kws={'label': 'final guess roost'}, cmap="crest")
    ax.set(xlabel="number of yellow letters", ylabel="number of green letters")
    ax.tick_params(top=True, labeltop=True, bottom=False, labelbottom=False)
    plt.savefig('count_roost.png')
    plt.clf()


def save_statistics():
    def save_heatmap(i, j, arr, label, filename, fmt=".4g"):
        arr = [[round(x, 1) for x in row] for row in arr]
        ax = sns.heatmap(arr, ax=axes[i][j], annot=True, fmt=fmt, cbar_kws={'label': label}, cmap="crest", vmin=0,
                         vmax=800, cbar=False)

        ax.tick_params(left=True, top=True, labeltop=True, labelbottom=False)

    def save_statistics_robot():
        def save_groups_count_robot():
            arr = [
                [0, 0, 0, 4, 37, 47],
                [0, 0, 10, 96, 225, 0],
                [1, 11, 139, 349, 0, 0],
                [9, 92, 355, 0, 0, 0],
                [99, 0, 0, 0, 0, 0],
                [841, 0, 0, 0, 0, 0]
            ]

            save_heatmap(0, 0, arr, "Size of the group", "robot_count.png")

        def save_groups_avg_robot():
            arr = [
                [0, 0, 0, 269.5, 87.2162, 19.1489],
                [0, 0, 94.4, 34.6458, 7.23111, 0],
                [64, 42.7273, 15.8489, 3.36103, 0, 0],
                [21.8889, 8.72826, 2, 0, 0, 0],
                [4.52525, 0, 0, 0, 0, 0],
                [1, 0, 0, 0, 0, 0]
            ]

            save_heatmap(1, 0, arr, "Average number of possible words", "robot_avg.png")

        def save_groups_max_robot():
            arr = [
                [0, 0, 0, 326, 190, 42],
                [0, 0, 133, 78, 18, 0],
                [64, 61, 30, 6, 0, 0],
                [25, 11, 2, 0, 0, 0],
                [6, 0, 0, 0, 0, 0],
                [1, 0, 0, 0, 0, 0]
            ]

            save_heatmap(2, 0, arr, "Maximal number of possible words", "robot_max.png")

        save_groups_avg_robot()
        save_groups_count_robot()
        save_groups_max_robot()

    def save_statistics_roost():
        def save_groups_count_roost():
            arr = [
                [0, 0, 0, 3, 35, 52],
                [0, 0, 11, 93, 232, 0],
                [1, 9, 125, 318, 0, 0],
                [8, 97, 370, 0, 0, 0],
                [112, 0, 0, 0, 0, 0],
                [849, 0, 0, 0, 0, 0]
            ]

            save_heatmap(0, 1, arr, "Size of the group", "roost_count.png")

        def save_groups_avg_roost():
            arr = [
                [0, 0, 0, 250.667, 86.6857, 18.3846],
                [0, 0, 90.7273, 32.2473, 7, 0],
                [64, 41.4444, 16.128, 3.42453, 0, 0],
                [21.5, 8.86598, 2, 0, 0, 0],
                [4.54464, 0, 0, 0, 0, 0],
                [1, 0, 0, 0, 0, 0]
            ]

            save_heatmap(1, 1, arr, "Average number of possible words", "roost_avg.png")

        def save_groups_max_roost():
            arr = [
                [0, 0, 0, 326, 190, 42],
                [0, 0, 133, 78, 18, 0],
                [64, 61, 30, 6, 0, 0],
                [25, 11, 2, 0, 0, 0],
                [6, 0, 0, 0, 0, 0],
                [1, 0, 0, 0, 0, 0]
            ]

            save_heatmap(2, 1, arr, "Maximal number of possible words", "roost_max.png")

        save_groups_avg_roost()
        save_groups_count_roost()
        save_groups_max_roost()
    sns.set(font_scale=0.8)

    fig, axes = plt.subplots(3, 2, figsize=(8, 8))

    save_statistics_robot()
    save_statistics_roost()

    yellow_lettes = "\n\nyellow letters"
    axes[0][0].set(ylabel="Group size" + yellow_lettes)
    axes[1][0].set(ylabel="Average number of\n coherent words" + yellow_lettes)
    axes[2][0].set(ylabel="Number of coherent words\n in the worst case" + yellow_lettes)

    green_letters = "\n\ngreen letters"
    axes[0][0].set_title("Solution with \"robot\"" + green_letters)
    axes[0][1].set_title("Solution with \"roost\"" + green_letters)

    for ax in axes.flat:
        ax.label_outer()

    plt.savefig("statistics")
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
    # save_letters_count1()
    save_statistics()

    # cands = get_worst(220)

    # unsolved = [("bobby", "booby"), ("paper", "parer"), ("truss", "trust"), ("crock", "crook")]
    #
    # print(solve_pairs([("catch", "hatch"), ("ample", "maple"), ("eagle", "legal"), ("droll", "drool"), ("abode",
    #                                                                                                  "adobe")]))
    # print(solve_pairs([("bobby", "booby"), ("dowry", "rowdy"), ("truss", "trust"), ("piper", "riper"),
    #                    ("crock", "crook")]))
    # print(solve_pairs([("bobby", "booby"), ("rogue", "rouge"), ("truss", "trust"), ("below", "elbow"), ("crock", "crook")]))
    # print(solve_pairs([("catch", "hatch"), ("ample", "maple"), ("eagle", "legal"), ("droll", "drool"), ("abode", "adobe")]))
    # print(solve_pairs([("catch", "hatch"), ("droll", "drool"), ("ample", "maple"), ("eagle", "legal"), ("abode", "adobe")]))
    # print(solve_pairs([("skate", "stake"), ("chili", "chill"), ("pasty", "patsy"), ("snoop", "spoon"), ("abode",
    #                                                                                                       "adobe")]))
    # print(solve_pairs([("abode", "adobe")]))
    #
    # print(solve_pairs(unsolved[1:]))
    # print(solve_pairs(unsolved[:1] + unsolved[2:]))
    # print(solve_pairs(unsolved[:2] + unsolved[3:]))
    # print(solve_pairs(unsolved[:3]))
    # print(solve_pairs(unsolved))