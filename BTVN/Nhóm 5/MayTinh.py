import sys
input = sys.stdin.readline

n, q = map(int, input().split())
a = input().split()

colors = ['B', 'G', 'R', 'Y']
idx = {c: i for i, c in enumerate(colors)}

nearestLeft = [[[ -1]*n for _ in range(4)] for _ in range(4)]
nearestRight = [[[ n]*n for _ in range(4)] for _ in range(4)]

for i1 in range(4):
    for i2 in range(i1, 4):
        pair = colors[i1] + colors[i2]

        # nearest left
        prev = -1
        for i in range(n):
            if a[i] == pair:
                prev = i
            nearestLeft[i1][i2][i] = prev

        # nearest right
        prev = n
        for i in range(n - 1, -1, -1):
            if a[i] == pair:
                prev = i
            nearestRight[i1][i2][i] = prev

for _ in range(q):
    i, j = map(int, input().split())
    if i > j: i, j = j, i
    i -= 1; j -= 1

    ans = 10**9
    for ci in a[i]:
        for cj in a[j]:
            x, y = sorted((idx[ci], idx[cj]))
            nl = nearestLeft[x][y]
            nr = nearestRight[x][y]

            if (i <= nr[i] <= j) or (i <= nl[j] <= j):
                ans = j - i
                break

            if nl[i] >= 0:
                ans = min(ans, i + j - 2 * nl[i])
            if nr[j] < n:
                ans = min(ans, 2 * nr[j] - i - j)

    print(-1 if ans == 10**9 else ans)
