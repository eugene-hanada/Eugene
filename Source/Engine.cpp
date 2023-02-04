#include "../Include/Engine.h"
#include <memory>
#include "../EugeneLib/Include/EugeneLib.h"
#include "../EugeneLib/Include/Math/Geometry.h"
#pragma comment(lib,"EugeneLib.lib")

std::unique_ptr<Eugene::System> libSys;
std::unique_ptr<Eugene::Graphics> graphics;
std::unique_ptr<Eugene::GpuEngine> gpuengine;
std::unique_ptr<Eugene::CommandList> cmdList;
std::unique_ptr<Eugene::GraphicsPipeline> gpipeLine;

// 頂点データ
std::unique_ptr <Eugene::GpuResource> vertexBuffer;
std::unique_ptr<Eugene::VertexView> vertexView;

// テクスチャデータ
std::unique_ptr <Eugene::GpuResource> textureBuffer;
std::unique_ptr < Eugene::ShaderResourceViews> textureView_;

// 行列データ
std::unique_ptr <Eugene::GpuResource> matrixBuffer;
std::unique_ptr < Eugene::ShaderResourceViews> matrixView_;

struct Vertex
{
	Eugene::Vector2 pos;
	Eugene::Vector2 uv;
};

int __cdecl  Run(const char* scene)
{
	return Eugene::Engine::GetInstance().Run();
}

Eugene::Engine& Eugene::Engine::GetInstance(void)
{
	static Engine instance;
	return instance;
}

int Eugene::Engine::Run(void)
{
	float color[]{ 0.0f,0.0f,0.0f,1.0f };
	while (libSys->Update())
	{
		cmdList->Begin();
		cmdList->SetRenderTarget(graphics->GetViews(), graphics->GetNowBackBufferIndex());
		cmdList->TransitionRenderTargetBegin(graphics->GetBackBufferResource());
		// レンダーターゲットをクリア
		cmdList->ClearRenderTarget(graphics->GetViews(), color, graphics->GetNowBackBufferIndex());

		cmdList->SetGraphicsPipeline(*gpipeLine);

		cmdList->SetShaderResourceView(*matrixView_, 0, 0);

		cmdList->SetShaderResourceView(*textureView_, 0, 1);

		cmdList->SetScissorrect({ 0,0 }, { 1280, 720 });

		cmdList->SetViewPort({ 0.0f,0.0f }, { 1280.0f, 720.0f });

		cmdList->SetPrimitiveType(Eugene::PrimitiveType::TriangleStrip);

		cmdList->SetVertexView(*vertexView);

		cmdList->Draw(4);

		cmdList->TransitionRenderTargetEnd(graphics->GetBackBufferResource());

		// コマンド終了
		cmdList->End();

		// 実行するコマンドを追加
		gpuengine->Push(*cmdList);

		// コマンドを実行
		gpuengine->Execute();

		// コマンド実行を待つ
		gpuengine->Wait();

		// スクリーンをバックバッファに入れ替えする
		graphics->Present();
	}
	return 0;
}

Eugene::Engine::Engine()
{
	libSys.reset(Eugene::CreateSystem({1280.0f, 720},u8"ゲーム"));
	Eugene::GpuEngine* tmp;
	graphics.reset(libSys->CreateGraphics(tmp));
	gpuengine.reset(tmp);
	cmdList.reset(graphics->CreateCommandList());

	// 頂点シェーダの入力のレイアウト
	std::vector<Eugene::ShaderInputLayout> layout
	{
		{"POSITION", 0, Eugene::Format::R32G32_FLOAT},
		{"TEXCOORD", 0, Eugene::Format::R32G32_FLOAT}
	};

	// シェーダー
	std::vector<std::pair<Eugene::Shader, Eugene::ShaderType>> shaders
	{
		{Eugene::Shader{"./Asset/vs.vso"}, Eugene::ShaderType::Vertex},
		{Eugene::Shader{"./Asset/ps.pso"}, Eugene::ShaderType::Pixel}
	};

	// レンダーターゲット
	std::vector<Eugene::RendertargetLayout> rendertargets
	{
		{Eugene::Format::R8G8B8A8_UNORM, Eugene::BlendType::Non}
	};

	std::vector<std::vector<Eugene::ShaderLayout>> shaderLayout
	{
		{Eugene::ShaderLayout{Eugene::ViewType::ConstantBuffer, 1,0}},
		{Eugene::ShaderLayout{Eugene::ViewType::Texture, 1,0}}
	};

	std::vector< Eugene::SamplerLayout> sampler
	{
		Eugene::SamplerLayout{}
	};

	gpipeLine.reset(graphics->CreateGraphicsPipeline(
		layout,
		shaders,
		rendertargets,
		Eugene::TopologyType::Triangle,
		false,
		shaderLayout,
		sampler
	));

	Vertex vertex[4]
	{
		{{0.0f,0.0f},{0.0f,0.0f}},
		{{246.0f,0.0f},{1.0f,0.0f}},
		{{0.0f, 246.0f},{0.0f, 1.0f}},
		{{246.0f,246.0f},{1.0f, 1.0f}}
	};
	std::unique_ptr <Eugene::GpuResource> upVertexBuffer;
	upVertexBuffer.reset(graphics->CreateUploadableResource(sizeof(Vertex) * 4));
	vertexBuffer.reset(graphics->CreateDefaultResource(sizeof(Vertex) * 4));
	upVertexBuffer.reset(graphics->CreateUploadableResource(sizeof(vertex)));
	auto ptr = upVertexBuffer->Map();
	std::copy(std::begin(vertex), std::end(vertex), reinterpret_cast<Vertex*>(ptr));
	upVertexBuffer->UnMap();
	vertexView.reset(graphics->CreateVertexView(sizeof(Vertex), 4ull, *vertexBuffer));

	Eugene::Image tex("./Asset/LogoPoly.png");
	std::unique_ptr <Eugene::GpuResource> upTextureBuffer;
	upTextureBuffer.reset(graphics->CreateUploadableTextureResource(tex));
	textureBuffer.reset(graphics->CreateTextureResource(tex.GetInfo()));
	textureView_.reset(graphics->CreateShaderResourceViews(1));
	textureView_->CreateTexture(*textureBuffer, 0);

	Eugene::Matrix4x4 matrix;
	Eugene::Get2DMatrix(matrix, { 1280.0f, 720.0f });
	std::unique_ptr <Eugene::GpuResource> upMatrixBuffer;
	upMatrixBuffer.reset(graphics->CreateUploadableResource(256));
	*static_cast<Eugene::Matrix4x4*>(upMatrixBuffer->Map()) = matrix;
	upMatrixBuffer->UnMap();

	matrixBuffer.reset(graphics->CreateDefaultResource(256));
	matrixView_.reset(graphics->CreateShaderResourceViews(1));
	matrixView_->CreateConstantBuffer(*matrixBuffer, 0);

	cmdList->Begin();
	cmdList->Copy(*vertexBuffer, *upVertexBuffer);
	cmdList->CopyTexture(*textureBuffer, *upTextureBuffer);
	cmdList->Copy(*matrixBuffer, *upMatrixBuffer);
	cmdList->End();
	gpuengine->Push(*cmdList);
	gpuengine->Execute();
	gpuengine->Wait();
}

Eugene::Engine::~Engine()
{

}
