#include <bits/stdc++.h>

struct edge {
    int u;
    int v;
    double p;
    bool alive;
};

std::string ok;

int n, m;
double L;
int s;
std::vector<double> c;
std::vector<edge> es;
std::vector<std::vector<std::pair<int, int>>> g;
std::vector<bool> checked;
std::vector<double> opt;
std::vector<double> conn;
std::vector<double> sc;
std::vector<int> move;

void input() {
    std::cin >> n >> m >> L;

    std::cin >> s;
    --s;

    c.resize(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> c[i];
    }

    g.resize(n);
    es.resize(m);
    checked.assign(n, false);
    
    for (int i = 0; i < m; ++i) {
        std::cin >> es[i].u >> es[i].v >> es[i].p;
        es[i].alive = false;
        --es[i].u;
        --es[i].v;

        g[es[i].u].emplace_back(es[i].v, i);
        g[es[i].v].emplace_back(es[i].u, i);
    }
}

void precalc() {
    const int FULL = (1 << n) - 1;

    conn.assign(1 << n, 0);

    for (int m = 1; m < (1 << n); ++m) {
        conn[m] = 1;
        int ft = m & (-m);
        for (int mp = (m - 1) & m; mp; mp = (mp - 1) & m) { // mp - подмаска m
            if (!(mp & ft)) continue;
            double val = 1;
            for (auto [u, v, p, _]: es) {
                if (!(m & (1 << u)) || !(m & (1 << v))) continue;
                if ((mp & (1 << u)) && (mp & (1 << v))) continue;
                if (!(mp & (1 << u)) && !(mp & (1 << v))) continue;
                val *= (1 - p);
            }
            conn[m] -= conn[mp] * val;
        }
    }

    sc.assign(1 << n, 0.0);
    for (int mask = 1; mask < (1 << n); ++mask) {
        int b = __builtin_ctz(mask);
        sc[mask] = sc[mask ^ (1 << b)] + c[b];
    }

    opt.assign(1 << n, 0);
    move.assign(1 << n, -1);

    for (int S = FULL; S >= 0; --S) {
        opt[S] = sc[S];
        move[S] = -1;

        int comp = FULL ^ S;
        if (comp == 0) continue;

        std::vector<double> sv(n, 0.0);

        for (int msk = comp; msk; msk = (msk - 1) & comp) {
            double prod = 1.0;

            for (auto [u, v, p, _] : es) {
                bool uin = msk & (1 << u);
                bool vin = msk & (1 << v);

                if (uin == vin) continue;

                int out = uin ? v : u;

                if (S & (1 << out)) continue;

                prod *= 1 - p;
            }

            double iso = conn[msk] * prod;

            double val = opt[S ^ msk] * iso;

            int bits = msk;
            while (bits) {
                int v = __builtin_ctz(bits);
                sv[v] += val;
                bits &= bits - 1;
            }
        }

        for (int i = 0; i < m; ++i) {
            auto [u, v, _, __] = es[i];

            bool uin = S & (1 << u);
            bool vin = S & (1 << v);

            if (uin == vin) continue;

            int out = uin ? v : u;

            double cand = -L + sv[out];

            if (cand > opt[S]) {
                opt[S] = cand;
                move[S] = i;
            }
        }
    }
}

void mmove(int ind) {
    std::cout << "move " << ind + 1 << std::endl;
    std::cin >> ok;
}

void mclear(int ind) {
    std::cout << "clear " << ind + 1 << std::endl;
    std::cin >> ok;
}

bool dfs(int v, int f, std::vector<bool> &used, std::vector<int> &path) {
    used[v] = true;
    if (v == f) {
        return true;
    }
    for (auto [u, ind]: g[v]) {
        if (es[ind].alive && checked[u] && !used[u]) {
            path.push_back(ind);
            if (dfs(u, f, used, path)) {
                return true;
            }
            path.pop_back();
        }
    }
    return false;
}

void move_to(int s, int f) {
    std::vector<int> path;
    std::vector<bool> used(n, false);
    dfs(s, f, used, path);
    for (auto ind: path) {
        mmove(ind);
    }
}

void explore(int v, int& mask) {
    std::cout << "look" << std::endl;
    for (int _ = 0; _ < g[v].size(); ++_) {
        int ind, status;
        std::cin >> ind >> status;
        --ind;
        es[ind].alive = status;
    }
    for (auto [u, ind]: g[v]) {
        if (es[ind].alive && !checked[u]) {
            mmove(ind);
            checked[u] = true;
            mask ^= (1 << u);
            explore(u, mask);
            mmove(ind);
        }
    }
}

void interactive() {
    int mask = (1 << s);
    int last = s;
    checked[s] = true;
    while (true) {
        move_to(s, last);
        explore(last, mask);
        if (move[mask] == -1) {
            std::cout << "end" << std::endl;
            break;
        } else {
            int ind = move[mask];
            int v = checked[es[ind].u] ? es[ind].u: es[ind].v;
            move_to(last, v);
            mclear(ind);
            es[ind].alive = true;
            last = checked[es[ind].u] ? es[ind].v: es[ind].u;
            mask ^= (1 << last);
            checked[last] = true;
        }
    }
}

int main() {
    input();
    precalc();
    interactive();
}