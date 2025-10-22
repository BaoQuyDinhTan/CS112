#include <iostream>
#include <vector>
#include <cmath>
#include <utility>
#include <iomanip>

using namespace std;

// ====== Cấu hình spiral ======
const float SQUARE_STEP = 150.0f;
const float HEX_RADIUS  = 50.0f;

// ====== Trạng thái toàn cục ======
bool inSpecial = false;
pair<float,float> center = {0, 0};

// Biến spiral vuông (giữ nguyên)
int sq_dir = 0, sq_len = 1, sq_cnt = 0;
float sq_x = 0, sq_y = 0;

// THAY ĐỔI: Xóa các biến spiral lục giác toàn cục cũ
// int hex_ring = 0;
// int hex_dir = 0;
// int hex_step = 0;
// int hex_q = 0, hex_r = 0;
// const pair<int,int> hex_dirs[6] = ... // Cái này cũng không cần nữa

// ====== Chuyển từ axial -> tọa độ thế giới ======
std::pair<float, float> axial_to_world(int q, int r, float hex_radius) {
    float x = hex_radius * (3.0f / 2.0f * q);
    float y = hex_radius * (sqrt(3.0f) / 2.0f * q + sqrt(3.0f) * r);
    return {x, y};
}

pair<float,float> nextSquareSpiral() {
    static const float dx[4] = {SQUARE_STEP, 0, -SQUARE_STEP, 0};
    static const float dy[4] = {0, SQUARE_STEP, 0, -SQUARE_STEP};

    sq_x += dx[sq_dir];
    sq_y += dy[sq_dir];
    sq_cnt++;

    if (sq_cnt == sq_len) {
        sq_cnt = 0;
        sq_dir = (sq_dir + 1) % 4;
        if (sq_dir % 2 == 0) sq_len++;
    }
    return {sq_x, sq_y};
}


class HexSpiralGenerator {
private:
    bool is_center;       
    int q, r;         
    int ring;            
    int side;             
    int step;               

    const std::vector<std::pair<int, int>> AXIAL_DIRECTIONS = {
        {-1, 0}, {-1, +1}, {0, +1}, {+1, 0}, {+1, -1}, {0, -1}
    };

public:
    HexSpiralGenerator() {
        reset();
    }

    void reset() {
        is_center = true;
        ring = 1;
        side = 0;
        step = 0;
        q = 1;
        r = -1;
    }

    std::pair<int, int> next() {
        if (is_center) {
            is_center = false;
            return {0, 0};
        }

        std::pair<int, int> current_pos = {q, r};

        auto [dq, dr] = AXIAL_DIRECTIONS[side];
        q += dq;
        r += dr;
        step++;

        if (step % ring == 0) {
            side = (side + 1) % 6; 
            if (side == 0) {
                ring++; 
                q=ring;
                r=-ring;
                step=0;
            }
        }

        return current_pos;
    }
}; 
HexSpiralGenerator spiral_gen;
pair<float,float> getNextPoint(float x, float y, int lastBossID) {
    static pair<float,float> current = {0, 0};
    static int iscenter = 1;
    if(iscenter)
    {
        current = {x,y};
        iscenter = 0;
        return current;
    }
    if (!inSpecial) {
        if (lastBossID == 36) {
            inSpecial = true;
            center = {x, y};    // Lưu lại tâm của vùng đặc biệt
            current = center;   // Vị trí đầu tiên trong vùng đặc biệt chính là tâm

            // THAY ĐỔI: Reset generator để bắt đầu vòng xoắn ốc mới từ tâm
            // THAY ĐỔI: Gọi next() một lần để "tiêu thụ" điểm tâm {0,0}.
            // Lần gọi getNextPoint tiếp theo sẽ bắt đầu từ điểm đầu tiên của vòng 1.
            spiral_gen.next();

            return current; // Trả về vị trí tâm ngay lập tức
        } else if (lastBossID == 0) {
            current = nextSquareSpiral();
        }
        // else: giữ nguyên `current` nếu hạ boss thường
        return current;
    }

    // Pha 2: Duyệt spiral lục giác quanh center
    if (lastBossID != 0) {
        return current; // Giữ nguyên vị trí nếu đang hạ boss
    }

    // Lấy tọa độ axial tiếp theo từ generator
    pair<int,int> next_axial_coords = spiral_gen.next();
    // Chuyển đổi sang tọa độ thế giới (tương đối so với tâm)
    auto [offset_x, offset_y] = axial_to_world(next_axial_coords.first, next_axial_coords.second, HEX_RADIUS);

    // Tính tọa độ tuyệt đối
    current.first = center.first + offset_x;
    current.second = center.second + offset_y;

    return current;
}

// Ý tưởng thuật toán:
//
// Phần 1: Tìm vùng đặc biệt:
// - Duyệt theo hình xoắn ốc với mỗi bước cách nhau 150 đơn vị.
// - Nếu hạ được boss ID 36, chuyển sang phần 2.
// - Nếu hạ được boss thường (ID != 0), giữ nguyên vị trí hiện tại để tiêu diệt hết boss trong vùng.
// - Nếu không hạ được boss, tiếp tục di chuyển theo spiral.
// Phần 2: Duyệt vùng đặc biệt:
// - Bắt đầu từ vị trí hạ được boss ID 36, chia bản đồ thành các hình lục giác đều với bán kính 50 đơn vị.
// - Lần lượt duyệt các vùng lục giác theo hình xoắn ốc với tâm là vị trí 36 đầu tiên.
// - Nếu hạ được boss thường, giữ nguyên vị trí để tiêu diệt hết boss trong vùng.
// - Nếu hạ được boss ID 36 khác, giữ nguyên vị trí và tiếp tục duyệt các vùng lục giác tiếp theo.
// - Nếu không hạ được boss, tiếp tục di chuyển theo spiral lục giác.

// Đảm bảo thuật toán:
// - Phần 1 Khả năng cao tìm được ít nhất một vùng đặc biệt nếu tồn tại
// - Phần 2 đảm bảo tiêu diệt mọi boss trong vùng
// - Vì đề đảm bảo bản đồ mỗi vùng là "rất lớn" nên việc duyệt phần 1 khả năng cao là không miss vùng đặc biệt và nếu đã tìm được vùng đặc biệt
//   thì việc duyệt phần 2 sẽ bao phủ toàn bộ vùng đặc biệt.
// - Việc giữ nguyên vị trí khi hạ boss thường đảm bảo không bỏ sót boss trong vùng đặc biệt
// Chia ô theo lục giác đều đảm bảo việc bao phủ vùng là tối ưu nhất với số bước di chuyển ít nhất,
// diện tích bị thừa mỗi bước chỉ khoảng 17% so với hình tròn bao quanh lục giác.
// int main()
// {
//     pair<float,float> pos = {0,0};
//     int lastBossID = 0;
//     for(int i=1;i<=100;i++)
//     {
//         pos = getNextPoint(pos.first, pos.second, lastBossID);
//         cout << fixed << setprecision(2) << pos.first << " " << pos.second << "\n";
//         cin>> lastBossID;
//         if(lastBossID !=0)  
//             cin >> pos.first >> pos.second; 
//     }
// }