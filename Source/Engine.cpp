#include "../Include/Engine.h"
#include <memory>
#include <semaphore>
#include <thread>
#include <atomic>
#include "../EugeneLib/Include/EugeneLib.h"
#include "../EugeneLib/Include/Math/Geometry.h"
#include "../EugeneLib/Include/Common/Debug.h"
#pragma comment(lib,"EugeneLib.lib")

std::unique_ptr<Eugene::System> libSys;
std::unique_ptr<Eugene::Graphics> graphics;
std::unique_ptr<Eugene::GpuEngine> gpuengine;
std::unique_ptr<Eugene::CommandList> gameCmdList;
std::unique_ptr<Eugene::CommandList> renderingCmdList;
std::unique_ptr<Eugene::GraphicsPipeline> gpipeLine;

// ���_�f�[�^
std::unique_ptr <Eugene::GpuResource> vertexBuffer;
std::unique_ptr<Eugene::VertexView> vertexView;

// �e�N�X�`���f�[�^
std::unique_ptr <Eugene::GpuResource> textureBuffer;
std::unique_ptr < Eugene::ShaderResourceViews> textureView_;

// �s��f�[�^
std::unique_ptr <Eugene::GpuResource> matrixBuffer;
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

	// ���_�f�[�^
	vertexBuffer.reset();
	vertexView.reset();

	// �e�N�X�`���f�[�^
	textureBuffer.reset();
	textureView_.reset();

	// �s��f�[�^
	matrixBuffer.reset();
	matrixView_.reset();

	graphics.reset();
	libSys.reset();
	return 0;
}

Eugene::Engine::Engine()
{
	libSys.reset(Eugene::CreateSystem({1280.0f, 720},u8"�Q�[��"));
	Eugene::GpuEngine* tmp;
	graphics.reset(libSys->CreateGraphics(tmp));
	gpuengine.reset(tmp);
	gameCmdList.reset(graphics->CreateCommandList());
	renderingCmdList.reset(graphics->CreateCommandList());

	// ���_�V�F�[�_�̓��͂̃��C�A�E�g
	std::vector<Eugene::ShaderInputLayout> layout
	{
		{"POSITION", 0, Eugene::Format::R32G32_FLOAT},
		{"TEXCOORD", 0, Eugene::Format::R32G32_FLOAT}
	};

	// �V�F�[�_�[
	std::vector<std::pair<Eugene::Shader, Eugene::ShaderType>> shaders
	{
		{Eugene::Shader{"./Asset/vs.vso"}, Eugene::ShaderType::Vertex},
		{Eugene::Shader{"./Asset/ps.pso"}, Eugene::ShaderType::Pixel}
	};

	// �����_�[�^�[�Q�b�g
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

	gameCmdList->Begin();
	gameCmdList->Copy(*vertexBuffer, *upVertexBuffer);
	gameCmdList->CopyTexture(*textureBuffer, *upTextureBuffer);
	gameCmdList->Copy(*matrixBuffer, *upMatrixBuffer);
	gameCmdList->End();
	gpuengine->Push(*gameCmdList);
	gpuengine->Execute();
	gpuengine->Wait();
}

Eugene::Engine::~Engine()
{
}

void Eugene::Engine::Rendering(void)
{
	float color[]{ 0.0f,0.0f,0.0f,1.0f };
	while (isRun.load())
	{
		gameCmdList->Begin();
		gameCmdList->SetRenderTarget(graphics->GetViews(), graphics->GetNowBackBufferIndex());
		gameCmdList->TransitionRenderTargetBegin(graphics->GetBackBufferResource());
		// �����_�[�^�[�Q�b�g���N���A
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

		// �R�}���h�I��
		gameCmdList->End();

		// �����_�����O���I����`����
		renderingSm.release();
		// �R�}���h���X���b�v�����܂őҋ@
		gameSm.acquire();
		DebugLog("�����_�����O�I��");
		// ���s����R�}���h��ǉ�
		gpuengine->Push(*renderingCmdList);

		// �R�}���h�����s
		gpuengine->Execute();

		// �R�}���h���s��҂�
		gpuengine->Wait();

		// �X�N���[�����o�b�N�o�b�t�@�ɓ���ւ�����
		graphics->Present();
	}
	
}

void Eugene::Engine::Game(void)
{
	float color[]{ 0.0f,0.0f,0.0f,1.0f };
	std::size_t idx = 0;
	isRun.store(libSys->Update());
	while (isRun.load())
	{
		DebugLog("�Q�[���A�b�v�f�[�g�I��");
		// �Q�[���A�b�v�f�[�g�X�e�[�W�I��
		
	
		// �����_�����O���I������܂őҋ@
		renderingSm.acquire();

		

		gameCmdList.swap(renderingCmdList);

		// �R�}���h���X���b�v�������Ƃ�`����
		gameSm.release();

		// �R�}���h��ς��swap����
		isRun.store(libSys->Update());
	}

	
}
