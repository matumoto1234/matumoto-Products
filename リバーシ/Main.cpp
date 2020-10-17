#include <Siv3D.hpp>

class GameBoard {
private:
	// 8x8 の二次元配列 (初期値は全要素 0)
	Grid<int32> m_grid = Grid<int32>(CellNum, CellNum);

	// ゲーム終了フラグ
	bool m_gameOver = false;

	// 格子を描く
	void drawGridLines() const {
		// 線を引く
		for (auto i : { 0, 1, 2, 3, 4, 5, 6, 7, 8 }) {
			Line(i * CellSize + CellWidthMargin, CellHeightMargin,
				i * CellSize + CellWidthMargin,
				CellNum * CellSize + CellHeightMargin)
				.draw(4, ColorF(0.25));

			Line(CellWidthMargin, i * CellSize + CellHeightMargin,
				CellNum * CellSize + CellWidthMargin,
				i * CellSize + CellHeightMargin)
				.draw(4, ColorF(0.25));
		}
	}

	// 盤面に色をつける
	void drawBoardColor() const {
		Rect(CellWidthMargin, CellHeightMargin, CellNum * CellSize,
			CellNum * CellSize)
			.draw(Palette::Seagreen);
	}

	bool CanPutPlaces(Point Cell, int32 color) const {
		int32 dirX[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
		int32 dirY[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
		bool doneFlag = false;

		for (int i = 0; i < 8; i++) {
			int32 nx = Cell.x;
			int32 ny = Cell.y;
			bool anotherColorFlag = false;
			while (1) {
				nx += dirX[i];
				ny += dirY[i];
				if (!rangeCheck(Point(nx, ny))) {
					// Print << nx << U" " << ny << U"まで来たよ";
					break;
				}
				if (m_grid[Point(nx, ny)] == 0) {
					break;
				}
				if (m_grid[Point(nx, ny)] != color) {
					anotherColorFlag = true;
					continue;
				}
				if (m_grid[Point(nx, ny)] == color) {
					if (anotherColorFlag) {
						doneFlag = true;
					}
					break;
				}
			}
			if (doneFlag) {
				break;
			}
		}
		return doneFlag;
	}

	// 置いた場所からタテヨコ斜めに裏返し処理
	bool reverseStones(const Point& Cell, int32 color) {
		// Print << Cell.x << U" " << Cell.y << U"におかれたよ！";
		int32 dirX[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
		int32 dirY[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
		bool doneFlag = false;

		for (int i = 0; i < 8; i++) {
			int32 nx = Cell.x;
			int32 ny = Cell.y;
			bool reverseFlag = false;
			bool anotherColorFlag = false;
			while (1) {
				nx += dirX[i];
				ny += dirY[i];
				if (!rangeCheck(Point(nx, ny))) {
					// Print << nx << U" " << ny << U"まで来たよ";
					break;
				}
				if (m_grid[Point(nx, ny)] == 0) {
					break;
				}
				if (m_grid[Point(nx, ny)] != color) {
					anotherColorFlag = true;
					continue;
				}
				if (m_grid[Point(nx, ny)] == color) {
					if (anotherColorFlag) {
						reverseFlag = true;
						doneFlag = true;
						break;
					}
					else {
						break;
					}
				}
			}
			if (!reverseFlag) {
				continue;
			}
			nx -= dirX[i];
			ny -= dirY[i];
			while (1) {
				if (nx == Cell.x && ny == Cell.y) {
					break;
				}
				// 石を置き換える
				m_grid[Point(nx, ny)] = color;

				// 石の数を足し引きする
				stoneCount(color, 1);
				if (color == X_Mark) {
					stoneCount(O_Mark, -1);
				}
				else {
					stoneCount(X_Mark, -1);
				}

				nx -= dirX[i];
				ny -= dirY[i];
			}
		}

		return doneFlag;
	}

	// おかれた場所が範囲内ならばtrue
	bool rangeCheck(const Point& Cell) const {
		if (Cell.y >= CellNum || Cell.y < 0 || Cell.x >= CellNum || Cell.x < 0) {
			return false;
		}
		else {
			return true;
		}
	}

	// セルを描く
	void drawCells(){

		bool canPutFlag = false;

		// 8x8 のセル
		for (auto p : step(Size(CellNum, CellNum))) {
			// セル
			const Rect cell = Rect(p * CellSize + Point(CellWidthMargin, CellHeightMargin), CellSize);

			// セルのマーク
			const int32 mark = m_grid[p];

			// 黒 マークだったら
			if (mark == X_Mark) {
				// 黒 マークを描く
				Circle(cell.center(), CellSize * 0.4 - 10).drawFrame(10, 0, ColorF(1.0, 1.0, 1.0));

				// このセルはこれ以上処理しない
				continue;
			}
			else if (mark == O_Mark) // 白 マークだったら
			{
				// 白 マークを描く
				Circle(cell.center(), CellSize * 0.4 - 10).drawFrame(10, 0, ColorF(0.2));

				// このセルはこれ以上処理しない
				continue;
			}

			// セルに石を置くことができるならば
			if (CanPutPlaces(p, m_currentMark)&&!m_gameOver) {
				// 灰　マークを描く
				Circle(cell.center(), CellSize * 0.4 - 10).drawFrame(10, 0, ColorF(0.5));
				canPutFlag = true;
			}

			// セルがマウスオーバーされたら
			if (cell.mouseOver()) {
				// カーソルを手のアイコンに
				Cursor::RequestStyle(CursorStyle::Hand);

				// セルの上に半透明の白を描く
				cell.stretched(-2).draw(ColorF(1.0, 0.6));
			}
		}
		if (!canPutFlag) {
			// 現在のマークを入れ替える
			m_currentMark = ((m_currentMark == O_Mark) ? X_Mark : O_Mark);
		}
	}

	// mode:1 -> マスがすべて埋まったとき mode:2 -> 石が 1 色のみかどうか
	bool winnerCheck(int32 mode) {
		bool gameOverFlag = false;

		switch (mode) {
		case 1:
			ClearPrint();
			gameOverFlag = true;
			if (O_MarkCnt > X_MarkCnt) {
				Print << U"白の勝ち!!";
			}
			else if (O_MarkCnt < X_MarkCnt) {
				Print << U"黒の勝ち!!";
			}
			else {
				Print << U"引き分け!!";
			}
			break;

		case 2:
			ClearPrint();
			if (X_MarkCnt == 0) {
				Print << U"白の勝ち!!";
				gameOverFlag = true;
			}
			else if (O_MarkCnt == 0) {
				Print << U"黒の勝ち!!";
				gameOverFlag = true;
			}
			break;

		default:
			Print << U"error";
			break;
		}

		return gameOverFlag;
	}

	//色に合わせて石の数を操作する
	void stoneCount(int32 color, int32 num) {
		if (color == X_Mark) {
			X_MarkCnt += num;
		}
		else if (color == O_Mark) {
			O_MarkCnt += num;
		}
	}

public:
	// セルの大きさ
	inline static const int32 CellSize = 50;
	// タテの余白
	inline static const int32 CellHeightMargin = 100;
	// ヨコの余白
	inline static const int32 CellWidthMargin = 200;
	// マスの数
	inline static const int32 CellNum = 8;
	// 白 マークの値
	inline static const int32 O_Mark = 1;
	// 黒 マークの値
	inline static const int32 X_Mark = 2;
	// 白 マークの数
	inline static int32 O_MarkCnt = 2;
	// 黒 マークの数
	inline static int32 X_MarkCnt = 2;
	// これから置くマーク
	inline static int32 m_currentMark = O_Mark;

	void update() {
		if (m_gameOver) {
			return;
		}
		// 8x8 のセル
		for (auto p : step(Size(CellNum, CellNum))) {
			// セル
			const Rect cell(p * CellSize + Point(CellWidthMargin, CellHeightMargin),
				CellSize);

			// セルのマーク
			const int32 mark = m_grid[p];

			

			// セルが空白で、なおかつクリックされたら
			if ((mark == 0) && cell.leftClicked()) {
				//石の裏返し処理が実行されないなら置けない
				if (reverseStones(p, m_currentMark) == false) {
					continue;
				}

				// セルに現在のマークを書き込む
				m_grid[p] = m_currentMark;

				//石の数を +1 する
				stoneCount(m_currentMark, 1);

				// 現在のマークを入れ替える
				m_currentMark = ((m_currentMark == O_Mark) ? X_Mark : O_Mark);

				// Print << U"現在の石の数は" << O_MarkCnt << U" " << X_MarkCnt;

				// 石が 1 色のみになったら
				if (winnerCheck(2)) {
					m_gameOver = true;
				}

				// 空白セルが 0 になったら
				if (m_grid.count(0) == 0) {
					// ゲーム終了
					m_gameOver = winnerCheck(1);
				}
			}
		}
	}

	//初期化
	void init() {
		m_grid[3][3] = O_Mark;
		m_grid[4][4] = O_Mark;
		m_grid[3][4] = X_Mark;
		m_grid[4][3] = X_Mark;
	}

	// ゲームをリセット
	void reset() {
		ClearPrint();

		m_currentMark = O_Mark;

		m_grid.fill(0);

		m_gameOver = false;
	}

	// 描画
	void draw(){
		drawBoardColor();

		drawGridLines();

		drawCells();
	}

	// ゲームが終了したかを返す
	bool isGameOver() const { return m_gameOver; }
};

void Main() {
	// 背景色
	Scene::SetBackground(Palette::Skyblue);

	GameBoard gameBoard;

	gameBoard.init();

	while (System::Update()) {
		gameBoard.update();

		gameBoard.draw();

		if (gameBoard.isGameOver()==false) {
			// 手番の表示
			ClearPrint();
			if (gameBoard.m_currentMark == gameBoard.O_Mark) {
				Print << U"Turn is Black";
			}
			else {
				Print << U"Turn is White";
			}
		}

		// ゲームが終了していたら
		if (gameBoard.isGameOver()) {
			// Reset ボタンを押せばリセット
			if (SimpleGUI::ButtonAt(U"Reset", Vec2(400, 520))) {
				gameBoard.reset();
				gameBoard.init();
			}
		}
	}
}