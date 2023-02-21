﻿#include "../Include/Engine.h"
#include <memory>
#include <semaphore>
#include <thread>
#include <atomic>
#include "../EugeneLib/Include/EugeneLib.h"
#include "../EugeneLib/Include/Math/Geometry.h"
#include "../EugeneLib/Include/Common/Debug.h"
#include "../Include/GameObject/Scene.h"

#include "../Include/ThreadPool.h"

#include <Windows.h>

#pragma comment(lib,"EugeneLib.lib")

std::unique_ptr<Eugene::System> libSys;
std::unique_ptr<Eugene::Graphics> graphics;
std::unique_ptr<Eugene::GpuEngine> gpuengine;
std::unique_ptr<Eugene::CommandList> gameCmdList;
std::unique_ptr<Eugene::CommandList> renderingCmdList;
std::unique_ptr<Eugene::GraphicsPipeline> gpipeLine;

// 頂点データ
std::unique_ptr <Eugene::BufferResource> vertexBuffer;
std::unique_ptr<Eugene::VertexView> vertexView;

// テクスチャデータ
std::unique_ptr <Eugene::ImageResource> textureBuffer;
std::unique_ptr < Eugene::ShaderResourceViews> textureView_;

// 行列データ
std::unique_ptr <Eugene::BufferResource> matrixBuffer;
std::unique_ptr < Eugene::ShaderResourceViews> matrixView_;

std::atomic_bool isRun{ true };

std::binary_semaphore gameSm{ 0 };
std::binary_semaphore renderingSm{ 0 };

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
	std::thread rendring{ &Engine::Rendering,this };
	Game();
	rendring.join();


	gpuengine.reset();
	gameCmdList.reset();
	gpipeLine.reset();

	// 頂点データ
	vertexBuffer.reset();
	vertexView.reset();

	// テクスチャデータ
	textureBuffer.reset();
	textureView_.reset();

	// 行列データ
	matrixBuffer.reset();
	matrixView_.reset();

	graphics.reset();
	libSys.reset();
	return 0;
}

Eugene::ThreadPool& Eugene::Engine::GetThreadPool(void)&
{
	return *threadPool_;
}

Eugene::Engine::Engine()
{
	threadPool_ = std::make_unique<ThreadPool>();

	libSys.reset(Eugene::CreateSystem({1280.0f, 720},u8"ゲーム"));
	Eugene::GpuEngine* tmp;
	graphics.reset(libSys->CreateGraphics(tmp));
	gpuengine.reset(tmp);
	gameCmdList.reset(graphics->CreateCommandList());
	renderingCmdList.reset(graphics->CreateCommandList());

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
	upVertexBuffer.reset(graphics->CreateUploadableBufferResource(sizeof(Vertex) * 4));
	vertexBuffer.reset(graphics->CreateBufferResource(sizeof(Vertex) * 4));
	auto ptr = upVertexBuffer->Map();
	std::copy(std::begin(vertex), std::end(vertex), reinterpret_cast<Vertex*>(ptr));
	upVertexBuffer->UnMap();
	vertexView.reset(graphics->CreateVertexView(sizeof(Vertex), 4ull, *vertexBuffer));

	Eugene::Image tex("./Asset/LogoPoly.png");
	std::unique_ptr <Eugene::GpuResource> upTextureBuffer;
	upTextureBuffer.reset(graphics->CreateBufferResource(tex));
	textureBuffer.reset(graphics->CreateImageResource(tex.GetInfo()));
	textureView_.reset(graphics->CreateShaderResourceViews(1));
	textureView_->CreateTexture(*textureBuffer, 0);

	Eugene::Matrix4x4 matrix;
	Eugene::Get2DMatrix(matrix, { 1280.0f, 720.0f });
	std::unique_ptr <Eugene::GpuResource> upMatrixBuffer;
	upMatrixBuffer.reset(graphics->CreateUploadableBufferResource(256));
	*static_cast<Eugene::Matrix4x4*>(upMatrixBuffer->Map()) = matrix;
	upMatrixBuffer->UnMap();

	matrixBuffer.reset(graphics->CreateBufferResource(256));
	matrixView_.reset(graphics->CreateShaderResourceViews(1));
	matrixView_->CreateConstantBuffer(*matrixBuffer, 0);

	gameCmdList->Begin();
	gameCmdList->Copy(*vertexBuffer, *upVertexBuffer);
	gameCmdList->CopyTexture(*textureBuffer, *upTextureBuffer);
	gameCmdList->Copy(*matrixBuffer, *upMatrixBuffer);
	gameCmdList->End();
	gpuengine->Push(*gameCmdList);
	gpuengine->Execute();
	gpuengine->Wait();

	/*auto h = threadPool_->AddTask([]() { Sleep(5000); DebugLog("終了"); });
	threadPool_->WaitAll();*/
}

Eugene::Engine::~Engine()
{
}

void Eugene::Engine::Rendering(void)
{
	while (isRun.load())
	{
		//DebugLog("レンダリング開始");
		// 実行するコマンドを追加
		gpuengine->Push(*renderingCmdList);

		// コマンドを実行
		gpuengine->Execute();

		// コマンド実行を待つ
		gpuengine->Wait();

		// スクリーンをバックバッファに入れ替えする
		graphics->Present();


		// レンダリングが終わりを伝える
		renderingSm.release();

		// コマンドがスワップされるまで待機
		gameSm.acquire();
	}
	
}

void Eugene::Engine::Game(void)
{
	float color[]{ 0.0f,0.0f,0.0f,1.0f };
	std::unique_ptr<Scene> scene = std::make_unique<Scene>();
	isRun.store(libSys->Update());
	while (isRun.load())
	{
		//DebugLog("ゲームアップデート開始");
		
		scene = scene->Update(std::move(scene));
		//DebugLog("ゲームアップデート終了");
		
		// レンダリングが終了するまで待機
		renderingSm.acquire();

		gameCmdList->Begin();
		gameCmdList->SetRenderTarget(graphics->GetViews(), graphics->GetNowBackBufferIndex());
		gameCmdList->TransitionRenderTargetBegin(graphics->GetBackBufferResource());
		// レンダーターゲットをクリア
		gameCmdList->ClearRenderTarget(graphics->GetViews(), color, graphics->GetNowBackBufferIndex());

		gameCmdList->SetGraphicsPipeline(*gpipeLine);

		gameCmdList->SetShaderResourceView(*matrixView_, 0, 0);

		gameCmdList->SetShaderResourceView(*textureView_, 0, 1);

		gameCmdList->SetScissorrect({ 0,0 }, { 1280, 720 });

		gameCmdList->SetViewPort({ 0.0f,0.0f }, { 1280.0f, 720.0f });

		gameCmdList->SetPrimitiveType(Eugene::PrimitiveType::TriangleStrip);

		gameCmdList->SetVertexView(*vertexView);

		gameCmdList->Draw(4);

		gameCmdList->TransitionRenderTargetEnd(graphics->GetBackBufferResource());

		// コマンド終了
		gameCmdList->End();

		gameCmdList.swap(renderingCmdList);
		//DebugLog(std::format("swap{0}", graphics->GetNowBackBufferIndex()));
		
		// コマンドをスワップしたことを伝える
		gameSm.release();

		// コマンドを積んでswapする
		isRun.store(libSys->Update());
	}
	gameSm.release();
	
}
