#include <Siv3D.hpp>
#include <filesystem>
constexpr int MAX_SENTENCE = 15;
constexpr int TYPING_TEXT_X = 150;
constexpr int TYPING_TEXT_Y = 250;
constexpr int LEVEL_1 = 106;
constexpr int LEVEL_2 = 157;
constexpr int LEVEL_3 = 208;
constexpr int LEVEL_4 = 276;
constexpr int LEVEL_5 = 330;
constexpr int LEVEL_6 = 380;
constexpr int LEVEL_7 = 430;
constexpr int LEVEL_8 = 480;
constexpr int LEVEL_9 = 530;
constexpr int LEVEL_10 = 600;


struct GameData
{
	int32 score;
	int32 sum_of_word_count;
	int32 number_of_mistypes;
	int32 rate;

	GameData() :
		score(0)
		, sum_of_word_count(0)
		, number_of_mistypes(0)
		, rate(0)
	{};

	void initialize() {
		score = 0;
		sum_of_word_count = 0;
		number_of_mistypes = 0;
		rate = 0;
	}

	int32 calculation_rate(int32 s) {
		if (s <= 0) {
			return 0;
		}
		int32 left, right;
		int32 rate_value;
		if (1 <= s && s <= LEVEL_1) {
			left = 1;
			right = LEVEL_1;
			rate_value = 1;
		}
		else if (LEVEL_1 + 1 <= s && s <= LEVEL_2) {
			left = LEVEL_1 + 1;
			right = LEVEL_2;
			rate_value = 400;
		}
		else if (LEVEL_2 + 1 <= s && s <= LEVEL_3) {
			left = LEVEL_2 + 1;
			right = LEVEL_3;
			rate_value = 800;
		}
		else if (LEVEL_3 + 1 <= s && s <= LEVEL_4) {
			left = LEVEL_3 + 1;
			right = LEVEL_4;
			rate_value = 1200;
		}
		else if (LEVEL_4 + 1 <= s && s <= LEVEL_5) {
			left = LEVEL_4 + 1;
			right = LEVEL_5;
			rate_value = 1600;
		}
		else if (LEVEL_5 + 1 <= s && s <= LEVEL_6) {
			left = LEVEL_5 + 1;
			right = LEVEL_6;
			rate_value = 2000;
		}
		else if (LEVEL_6 + 1 <= s && s <= LEVEL_7) {
			left = LEVEL_6 + 1;
			right = LEVEL_7;
			rate_value = 2400;
		}
		else if (LEVEL_7 + 1 <= s && s <= LEVEL_8) {
			left = LEVEL_7 + 1;
			right = LEVEL_8;
			rate_value = 2800;
		}
		else if (LEVEL_8 + 1 <= s && s <= LEVEL_9) {
			left = LEVEL_8 + 1;
			right = LEVEL_9;
			rate_value = 3200;
		}
		else{
			left = LEVEL_9 + 1;
			right = LEVEL_10;
			rate_value = 3600;
		}
		double one = (double)(right - left) / 100.0;
		double wariai = ((double)(s - left) / one);
		return (int32)((double)rate_value + wariai * 4.0);
	}

	void calculation_score(double time) {
		int32 all_types = number_of_mistypes + sum_of_word_count;
		double WPM = 60.0 * (double)all_types / time;
		double correct_rate = (double)sum_of_word_count / (double)all_types;
		//Print << U"correct_rate=" << correct_rate << U" all_types=" << all_types;
		//Print << U"WPM=" << WPM<<U" sumWord="<<sum_of_word_count;
		score = (int32)(WPM * correct_rate * correct_rate * correct_rate);
		rate = calculation_rate(score);
	}
};





using App = SceneManager<String, GameData>;









class TypingTexts {
private:

	String temp;
	TextReader reader;

public:
	Array<String> m_texts;
	String m_input;
	String m_target;

	TypingTexts() :reader(U"./../text.txt") {

		if (!reader) {
			throw Error(U"Failed to open `text.txt` \n Please set at `{thisApp}/../text.txt`");
		}


		while (reader.readLine(temp)) {
			TypingTexts::m_texts << temp;
		}

		m_target = m_texts.choice();
	}
};

struct GameStatus {

	int32 sentenceCounter;
	Stopwatch CountdownToTheStart;
	Stopwatch typing_time;
	bool isPressedSpaceKey;
	bool isStartedFlag;

	GameStatus() :
		sentenceCounter(0)
		, isPressedSpaceKey(false)
		, isStartedFlag(false)
	{};

	void Reset() {
		sentenceCounter = 0;
		isPressedSpaceKey = false;
		isStartedFlag = false;
		typing_time.reset();
		CountdownToTheStart.reset();
	}

	void Start() {
		typing_time.start();
		isStartedFlag = true;
	}

	void PressedSpace() {
		isPressedSpaceKey = true;
		CountdownToTheStart.start();
	}

};







class Title : public App::Scene
{
public:

	Title(const InitData& init) : IScene(init) {
		getData().initialize();
	}

	void update() override
	{
		if (MouseL.down())
		{
			changeScene(U"Game");
		}
	}

	void draw() const override
	{
		Scene::SetBackground(ColorF(0.3, 0.4, 0.5));

		FontAsset(U"TitleFont")(U"Typing Game\n(C/C++)").drawAt(400, 100);

		FontAsset(U"TitleFont")(U"Left Click!!\n").drawAt(400, 400);

		Circle(Cursor::Pos(), 50).draw(ColorF(1.0, 0.4, 0.5, 0.5));
	}
};

class Game : public App::Scene
{
private:

	Texture m_texture;

	TypingTexts m_typingTexts;

	GameStatus m_status;

	Point m_text_xy;

public:

	Game(const InitData& init) : IScene(init)
		, m_texture(Emoji(U"🐈"))
		, m_text_xy(Point(TYPING_TEXT_X, TYPING_TEXT_Y))
	{

	}

	void update() override
	{

		if (KeySpace.down()) {
			m_status.PressedSpace();
		}

		if (!m_status.isPressedSpaceKey) {
			FontAsset(U"InGameTextFont")(U"スペースキーを押すと始まります。").drawAt(400, 300);
			return;
		}

		if (!m_status.isStartedFlag && m_status.CountdownToTheStart.s() < 3) {
			FontAsset(U"CountdownFont")(3 - m_status.CountdownToTheStart.s()).drawAt(400, 300);
			return;
		}
		else if (!m_status.isStartedFlag) {
			m_status.Start();
		}


		// テキスト入力（TextInputMode::DenyControl: エンターやタブ、バックスペースは受け付けない）
		TextInput::UpdateText(m_typingTexts.m_input, TextInputMode::DenyControl);

		// 誤った入力が含まれていたら削除
		while (!m_typingTexts.m_target.starts_with(m_typingTexts.m_input))
		{
			// ミスタイプ数をカウント
			getData().number_of_mistypes++;
			// 誤った入力を削除
			m_typingTexts.m_input.pop_back();
		}


		// 一致したら次の問題へ
		if (m_typingTexts.m_input == m_typingTexts.m_target)
		{
			// 文字数などをカウント
			m_status.sentenceCounter++;
			getData().sum_of_word_count += (int32)m_typingTexts.m_input.size();


			// タイピングテキストを更新
			m_typingTexts.m_target = m_typingTexts.m_texts.choice();
			m_typingTexts.m_input.clear();


			// エンディングに移動
			if (m_status.sentenceCounter >= MAX_SENTENCE) {
				getData().calculation_score(m_status.typing_time.sF());
				m_status.Reset();
				changeScene(U"Ending", 1);
			}
		}

		// 文字が画面の外に出ると左にずらしていく
		if (m_typingTexts.m_input.size() >= 25) {
			int mojisu = (int)m_typingTexts.m_input.size() - 25;
			m_text_xy.x = TYPING_TEXT_X - (mojisu * 20);
			m_text_xy.y = TYPING_TEXT_Y;
		}
		else {
			m_text_xy.x = TYPING_TEXT_X;
			m_text_xy.y = TYPING_TEXT_Y;
		}

	}

	void draw() const override
	{
		Scene::SetBackground(ColorF(0.2, 0.8, 0.6));

		m_texture.drawAt(Cursor::Pos());

		// 秒数を表示
		FontAsset(U"InGameTextFont")(U"Time: {}"_fmt(m_status.typing_time.s())).draw(20, 20);
		FontAsset(U"InGameTextFont")(U"Remain: {}"_fmt(MAX_SENTENCE - m_status.sentenceCounter)).draw(20, 80);

		// スタートしている場合のみ出力する
		if (m_status.isStartedFlag) {
			FontAsset(U"TypingFont")(m_typingTexts.m_target).draw(m_text_xy, ColorF(0.75));
			FontAsset(U"TypingFont")(m_typingTexts.m_input).draw(m_text_xy, ColorF(0.1));
		}
	}
};

class Ending : public App::Scene {
private:
	Texture hat;
	Texture professor;
	Texture medal;
	Texture bronze_medal;
	Texture silver_medal;
	Texture king;

public:

	Ending(const InitData& init)
		:IScene(init)
		, hat(Emoji(U"🎩"))
		, professor(Emoji(U"🎓"))
		, medal(Emoji(U"🎖"))
		, bronze_medal(Emoji(U"🥉"))
		, silver_medal(Emoji(U"🥈"))
		, king(Emoji(U"🥇👑"))
	{

	}

	Color rate_to_color(int32 rate) const {
		if (rate <= 0) return Color(0, 0, 0);
		if (1 <= rate && rate <= LEVEL_1) return Color(153, 153, 153);
		if (LEVEL_1 + 1 <= rate && rate <= LEVEL_2) return Color(152, 38, 0);
		if (LEVEL_2 + 1 <= rate && rate <= LEVEL_3) return Color(0, 203, 50);
		if (LEVEL_3 + 1 <= rate && rate <= LEVEL_4) return Color(0, 191, 255);
		if (LEVEL_4 + 1 <= rate && rate <= LEVEL_5) return Color(0, 0, 255);
		if (LEVEL_5 + 1 <= rate && rate <= LEVEL_6) return Color(255, 255, 0);
		if (LEVEL_6 + 1 <= rate && rate <= LEVEL_7) return Color(255, 153, 50);
		return Color(255, 0, 63);
	}

	void rate_to_Medal(int32 rate) const {
		if (rate <= 0) FontAsset(U"MedalFont")(U"黒色級").drawAt(Scene::Center());
		else if (1 <= rate && rate <= LEVEL_1)  FontAsset(U"MedalFont")(U"灰色級").drawAt(Scene::Center());
		else if (LEVEL_1 + 1 <= rate && rate <= LEVEL_2) FontAsset(U"MedalFont")(U"茶色級").drawAt(Scene::Center());
		else if (LEVEL_2 + 1 <= rate && rate <= LEVEL_3)FontAsset(U"MedalFont")(U"緑色級").drawAt(Scene::Center());
		else if (LEVEL_3 + 1 <= rate && rate <= LEVEL_4)FontAsset(U"MedalFont")(U"水色級").drawAt(Scene::Center());
		else if (LEVEL_4 + 1 <= rate && rate <= LEVEL_5)FontAsset(U"MedalFont")(U"青色級").drawAt(Scene::Center());
		else if (LEVEL_5 + 1 <= rate && rate <= LEVEL_6) {
			FontAsset(U"MedalFont")(U"黄色級").drawAt(Scene::Center());
			hat.drawAt(400, 200);
		}
		else if (LEVEL_6 + 1 <= rate && rate <= LEVEL_7) {
			FontAsset(U"MedalFont")(U"橙色級").drawAt(Scene::Center());
			professor.drawAt(400, 200);
		}
		else if (LEVEL_7 + 1 <= rate && rate <= LEVEL_8) {
			FontAsset(U"MedalFont")(U"赤色級").drawAt(Scene::Center());
			medal.drawAt(400, 200);
		}
		else if (LEVEL_8 + 1 <= rate && rate <= LEVEL_9) {
			FontAsset(U"MedalFont")(U"赤色級(銅冠)!!!!").drawAt(Scene::Center());
			bronze_medal.drawAt(400, 200);
		}
		else if (LEVEL_9 + 1 <= rate && rate <= LEVEL_10) {
			FontAsset(U"MedalFont")(U"赤色級(銀冠)!!!!").drawAt(Scene::Center());
			silver_medal.drawAt(400, 200);
		}
		else {
			FontAsset(U"MedalFont")(U"タイピング界のtourist").drawAt(Scene::Center());
			king.drawAt(400, 200);
		}
	}


	void update() override {
		if (SimpleGUI::Button(U"Go to Title", Vec2(300, 500), 200)) {
			changeScene(U"Title", 1);
		}
	}

	void draw() const override
	{
		Scene::SetBackground(rate_to_color(getData().score));

		rate_to_Medal(getData().score);

		FontAsset(U"TitleFont")(U"あなたのタイピング力は...").drawAt(400, 100);

		FontAsset(U"ScoreFont")(U"Score: {}"_fmt(getData().score)).draw(520, 540);

		FontAsset(U"ScoreFont")(U"Rating: {}"_fmt(getData().rate)).draw(520, 500);

		Circle(Cursor::Pos(), 50).draw(ColorF(1.0, 0.4, 0.5, 0.5));
	}
};



void Main()
{
	FontAsset::Register(U"TitleFont", 60, Typeface::Heavy);
	FontAsset::Register(U"ScoreFont", 30, Typeface::Bold);
	FontAsset::Register(U"TypingFont", 40, Typeface::Bold);
	FontAsset::Register(U"InGameTextFont", 40, Typeface::Light);
	FontAsset::Register(U"CountdownFont", 60, Typeface::Bold);
	FontAsset::Register(U"MedalFont", 80, Typeface::Bold);

	App manager;

	manager.add<Title>(U"Title");
	manager.add<Game>(U"Game");
	manager.add<Ending>(U"Ending");

	while (System::Update())
	{
		if (!manager.update())
		{
			break;
		}

	}
}