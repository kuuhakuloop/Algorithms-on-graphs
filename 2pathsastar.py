import heapq

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.animation import FuncAnimation, PillowWriter
from matplotlib.patches import Rectangle

SIZE = 50
GIF_FPS = 12
TARGET_GIF_SECONDS = 36
FINAL_HOLD_SECONDS = 2
MAX_GIF_FRAMES = GIF_FPS * TARGET_GIF_SECONDS
FINAL_HOLD_FRAMES = GIF_FPS * FINAL_HOLD_SECONDS

grid = np.zeros((SIZE, SIZE), dtype=int)


def add_block(top, left, height, width):
    for row in range(top, top + height):
        for col in range(left, left + width):
            if 0 <= row < SIZE and 0 <= col < SIZE:
                grid[row, col] = 1


# ================= СТЕНЫ =================
vertical_walls = [8, 16, 24, 32, 40]
vertical_gaps = {
    8: [(5, 10), (22, 27), (40, 45)],
    16: [(12, 17), (30, 35)],
    24: [(4, 9), (20, 26), (38, 43)],
    32: [(10, 15), (28, 33)],
    40: [(6, 12), (24, 29), (41, 46)],
}

for col in vertical_walls:
    for row in range(1, SIZE - 1):
        if any(start <= row <= end for start, end in vertical_gaps[col]):
            continue
        grid[row, col] = 1

horizontal_walls = [11, 19, 27, 35, 43]
horizontal_gaps = {
    11: [(3, 7), (18, 22), (34, 38)],
    19: [(8, 13), (25, 29), (42, 46)],
    27: [(4, 8), (20, 24), (36, 41)],
    35: [(10, 15), (27, 31), (43, 47)],
    43: [(6, 11), (22, 26), (38, 44)],
}

for row in horizontal_walls:
    for col in range(1, SIZE - 1):
        if any(start <= col <= end for start, end in horizontal_gaps[row]):
            continue
        grid[row, col] = 1

# ================= КРУПНЫЕ ПРЕПЯТСТВИЯ =================
blocks = [
    (4, 4, 3, 3),
    (6, 20, 4, 3),
    (7, 44, 3, 3),
    (14, 12, 3, 4),
    (15, 34, 4, 4),
    (22, 5, 4, 4),
    (23, 27, 3, 5),
    (29, 17, 4, 4),
    (30, 41, 4, 3),
    (37, 6, 4, 5),
    (38, 23, 3, 4),
    (39, 45, 3, 3),
]

for block in blocks:
    add_block(*block)

# ================= ТОЧКИ =================
start1 = (46, 3)
end1 = (3, 46)
start2 = (47, 3)
end2 = (4, 46)

for point in (start1, start2, end1, end2):
    grid[point] = 0


# ================= A* =================
def heuristic_pair(state):
    p1, p2 = state
    return (
        abs(p1[0] - end1[0])
        + abs(p1[1] - end1[1])
        + abs(p2[0] - end2[0])
        + abs(p2[1] - end2[1])
    )


def is_valid(pos):
    row, col = pos
    return 0 <= row < SIZE and 0 <= col < SIZE and grid[row, col] != 1


def get_neighbors(state, occ1, occ2):
    p1, p2 = state
    moves = [(0, 1), (0, -1), (1, 0), (-1, 0)]
    result = []

    for dr1, dc1 in moves:
        np1 = (p1[0] + dr1, p1[1] + dc1)
        if not is_valid(np1):
            continue

        for dr2, dc2 in moves:
            np2 = (p2[0] + dr2, p2[1] + dc2)
            if not is_valid(np2):
                continue

            if np1 == np2:
                continue
            if np1 == p2 and np2 == p1:
                continue

            # Не даём путям пересекать уже пройденные клетки друг друга.
            if np1 in occ2 or np2 in occ1:
                continue

            dist_penalty = abs(np1[0] - np2[0]) + abs(np1[1] - np2[1])
            result.append((np1, np2, dist_penalty))

    return result


def astar():
    start = (start1, start2)
    goal = (end1, end2)

    open_set = []
    heapq.heappush(open_set, (0, start))

    came = {}
    g = {start: 0}

    occ1 = {start: {start1}}
    occ2 = {start: {start2}}

    visited_all_1 = {start1}
    visited_all_2 = {start2}

    frames = []
    step = 0

    while open_set:
        _, current = heapq.heappop(open_set)
        step += 1

        frames.append(
            {
                "visited1": visited_all_1.copy(),
                "visited2": visited_all_2.copy(),
                "curr1": current[0],
                "curr2": current[1],
                "occ1": occ1[current].copy(),
                "occ2": occ2[current].copy(),
                "step": step,
            }
        )

        if current == goal:
            path1, path2 = [], []
            state = current

            while state in came:
                path1.append(state[0])
                path2.append(state[1])
                state = came[state]

            path1.append(start1)
            path2.append(start2)
            path1.reverse()
            path2.reverse()

            for _ in range(15):
                frames.append(
                    {
                        "final": True,
                        "path1": path1,
                        "path2": path2,
                        "visited1": visited_all_1,
                        "visited2": visited_all_2,
                    }
                )

            return path1, path2, frames

        for np1, np2, extra_cost in get_neighbors(current, occ1[current], occ2[current]):
            visited_all_1.add(np1)
            visited_all_2.add(np2)

            new_state = (np1, np2)
            new_g = g[current] + 1 + extra_cost

            if new_state not in g or new_g < g[new_state]:
                g[new_state] = new_g
                f = new_g + heuristic_pair(new_state)
                heapq.heappush(open_set, (f, new_state))
                came[new_state] = current

                occ1[new_state] = occ1[current] | {np1}
                occ2[new_state] = occ2[current] | {np2}

    return None, None, frames


def compress_frames(frames):
    if not frames:
        return frames

    final_frame = frames[-1] if frames[-1].get("final", False) else None
    search_frames = [frame for frame in frames if not frame.get("final", False)]

    reserved_frames = FINAL_HOLD_FRAMES if final_frame else 0
    search_budget = max(1, MAX_GIF_FRAMES - reserved_frames)

    if len(search_frames) > search_budget:
        indices = np.linspace(0, len(search_frames) - 1, search_budget, dtype=int)
        unique_indices = []
        previous = None

        for idx in indices.tolist():
            if idx != previous:
                unique_indices.append(idx)
                previous = idx

        if unique_indices[-1] != len(search_frames) - 1:
            unique_indices[-1] = len(search_frames) - 1

        compressed = [search_frames[idx] for idx in unique_indices]
    else:
        compressed = search_frames

    if final_frame:
        compressed.extend([final_frame] * FINAL_HOLD_FRAMES)

    return compressed


# ================= RUN =================
p1, p2, frames = astar()
if p1 is None or p2 is None:
    raise RuntimeError("A* не нашёл два непересекающихся пути на текущей карте.")

frames = compress_frames(frames)
print(f"Кадров для GIF: {len(frames)}, длительность примерно {len(frames) / GIF_FPS:.1f} сек.")


# ================= ANIMATION =================
fig, ax = plt.subplots(figsize=(14, 14))


def draw(frame_idx):
    ax.clear()

    for idx in range(SIZE + 1):
        ax.axhline(idx - 0.5, color="lightgray", lw=0.25)
        ax.axvline(idx - 0.5, color="lightgray", lw=0.25)

    for row in range(SIZE):
        for col in range(SIZE):
            if grid[row, col] == 1:
                ax.add_patch(Rectangle((col - 0.5, row - 0.5), 1, 1, color="#40E0D0"))

    frame = frames[min(frame_idx, len(frames) - 1)]

    for row, col in frame.get("visited1", []):
        ax.add_patch(Rectangle((col - 0.5, row - 0.5), 1, 1, color="#FFCCCC", alpha=0.2))
    for row, col in frame.get("visited2", []):
        ax.add_patch(Rectangle((col - 0.5, row - 0.5), 1, 1, color="#CCCCFF", alpha=0.2))

    for row, col in frame.get("occ1", []):
        ax.add_patch(
            Rectangle(
                (col - 0.5, row - 0.5),
                1,
                1,
                facecolor="#FF6666",
                alpha=0.6,
                edgecolor="#CC0000",
                linewidth=0.8,
            )
        )

    for row, col in frame.get("occ2", []):
        ax.add_patch(
            Rectangle(
                (col - 0.5, row - 0.5),
                1,
                1,
                facecolor="#6666FF",
                alpha=0.6,
                edgecolor="#0000CC",
                linewidth=0.8,
            )
        )

    if not frame.get("final", False):
        row, col = frame["curr1"]
        ax.add_patch(Rectangle((col - 0.5, row - 0.5), 1, 1, color="red", zorder=4))
        row, col = frame["curr2"]
        ax.add_patch(Rectangle((col - 0.5, row - 0.5), 1, 1, color="blue", zorder=4))

    if frame.get("final", False):
        for idx in range(len(frame["path1"]) - 1):
            r1, c1 = frame["path1"][idx]
            r2, c2 = frame["path1"][idx + 1]
            ax.plot([c1, c2], [r1, r2], color="red", lw=4)

        for idx in range(len(frame["path2"]) - 1):
            r1, c1 = frame["path2"][idx]
            r2, c2 = frame["path2"][idx + 1]
            ax.plot([c1, c2], [r1, r2], color="blue", lw=4)

    ax.scatter(start1[1], start1[0], color="darkred", s=140)
    ax.scatter(start2[1], start2[0], color="darkblue", s=140)
    ax.scatter(end1[1], end1[0], color="red", s=140, marker="s")
    ax.scatter(end2[1], end2[0], color="blue", s=140, marker="s")

    ax.set_xlim(-0.5, SIZE - 0.5)
    ax.set_ylim(-0.5, SIZE - 0.5)
    ax.invert_yaxis()
    ax.set_aspect("equal")
    ax.axis("off")


anim = FuncAnimation(fig, draw, frames=len(frames), interval=1000 // GIF_FPS)
anim.save("result.gif", writer=PillowWriter(fps=GIF_FPS))

plt.show()
