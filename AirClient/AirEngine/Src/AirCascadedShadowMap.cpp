#include "AirCascadedShadowMap.h"
#include "AirEngineMaterial.h"
#include "AirEngineSystem.h"
#include "AirEngineCamera.h"
#include "AirQuadRenderable.h"
namespace	Air{
	namespace	Client{


		CascadedShadowMap::CascadedShadowMap()
		{
			m_pMainCamera	=	NULL;
			m_pShadowDepth	=	NULL;
			m_pShadowDepthTemp	=	NULL;
			m_pShadowMask	=	NULL;
			m_pMaskMaterial	=	NULL;
			m_pQuad			=	NULL;
			m_pBlurX		=	NULL;
			m_pBlurY		=	NULL;

			m_vLightDirection	=	Float3(1,-1,1);
			m_vLightDirection.Normalize();
		}

		CascadedShadowMap::~CascadedShadowMap()
		{

		}

		Air::U1 CascadedShadowMap::Init( Camera* pMainCamera,U32 uiSplite /*= 4*/ )
		{
			if(uiSplite==0)
				return false;

			m_pQuad			=	new QuadRenderable();

			m_pMainCamera	=	pMainCamera;
			RenderTarget::Info rtinfo;
			rtinfo.SetSingleTarget(512,512,enTFMT_R32_FLOAT,true);
			m_pShadowDepth	=	RenderSystem::GetSingleton()->CreateProduct<RenderTarget*>("CascadedShadowMapDepth","Target",&rtinfo);
			m_pShadowDepth->SetClearFlag(true,true,true);
			m_pShadowDepth->AddPhaseFlag(enPI_Shadow);
			m_pShadowDepth->SetBKColor(Float4(1,1,1,1));
			//rtinfo.SetSingleTarget(512,512,enTFMT_R32G32_FLOAT);
			//m_pShadowDepthTemp	=	RenderSystem::GetSingleton()->CreateProduct<RenderTarget*>("CascadedShadowMapDepthTemp","Target",&rtinfo);
			//m_pShadowDepthTemp->SetClearFlag(true,true,true);
			//m_pShadowDepthTemp->AddPhaseFlag(enPI_Shadow);
			//m_pShadowDepthTemp->SetBKColor(Float4(1,1,1,1));


			
			rtinfo.SetSingleTargetScreen(enTFMT_R8G8B8A8_UNORM,1.0f,true,RenderSystem::GetSingleton()->GetMainWindow());
			m_pShadowMask	=	RenderSystem::GetSingleton()->CreateProduct<RenderTarget*>("CascadedShadowMapMask","Target",&rtinfo);
			m_pShadowMask->SetClearFlag(false,true,false);
			m_pShadowMask->SetBKColor(Float4(1,1,1,1));

			Material::Info	blurinfo;
			blurinfo.strTemplate	=	"MT_BlurX";
			m_pBlurX		=	EngineSystem::GetSingleton()->CreateProduct<Material*>("BlurX","Material",&blurinfo);
			blurinfo.strTemplate	=	"MT_BlurY";
			m_pBlurY		=	EngineSystem::GetSingleton()->CreateProduct<Material*>("BlurY","Material",&blurinfo);

			m_pMaskMaterial	=	EngineSystem::GetSingleton()->CreateProduct<Material*>("ShadowMask","Material");


			m_vecCSMCamera.resize(uiSplite);
			AString	strCameraName	=	"CascadedShadowCamera";
			Light::Info info;
			info.SetDirection(m_vLightDirection);
			for(U32 i=0;i<uiSplite;i++){
				m_vecCSMCamera[i]	=	EngineSystem::GetSingleton()->CreateProduct<Light*>(strCameraName+Common::Converter::ToString(i),"Light",&info);
				m_vecCSMCamera[i]->SetDir(m_vLightDirection);
				m_vecCSMCamera[i]->SetOrtho(true);
				float Scale	=	pow(4.0f,(float)(i+1))*4.0f;
	

				m_vecCSMCamera[i]->SetWidth(Scale);
				m_pShadowDepth->AddCamera(m_vecCSMCamera[i],i);
			}
			return true;
		}

		Air::U1 CascadedShadowMap::Release()
		{
			SAFE_RELEASE_REF(m_pShadowDepth);
			SAFE_RELEASE_REF(m_pShadowDepthTemp);
			SAFE_RELEASE_REF(m_pShadowMask);
			SAFE_RELEASE_REF(m_pMaskMaterial);
			SAFE_DELETE(m_pQuad);
			SAFE_RELEASE_REF(m_pBlurX);
			SAFE_RELEASE_REF(m_pBlurY);
			U32	uiSize	=	m_vecCSMCamera.size();
			for(U32 i=0;i<uiSize;i++){
				SAFE_RELEASE_REF(m_vecCSMCamera[i]);
			}
			return true;
		}

		Air::U1 CascadedShadowMap::UpdateCamera( Camera* pMainCamera )
		{
			Float3 vCamPos	=	m_pMainCamera->GetPosition();
			Float3 vCamDir	=	m_pMainCamera->GetDir();

			for(int i=0;i<m_vecCSMCamera.size();i++){
				Light*	pLight	=	m_vecCSMCamera[i];

				float fScale	=	pow(4.0f,(float)(i+1))*2.0f;
				float fScale2	=	fScale*2/256.0f;
				Float3 pos		=	vCamPos+vCamDir*fScale-m_vLightDirection*200;

				Float44	view = pLight->GetViewMatrix();
				Float3 viewpos	=	view*pos;
				view.Inverse();
				viewpos/=fScale2;
				viewpos	=	Float3(floor(viewpos.x),floor(viewpos.y),floor(viewpos.z))*fScale2;
				Float3 vpos	=	view*viewpos;
				pLight->SetPosition(vpos);
			}


			return true;
		}

		Air::U1 CascadedShadowMap::UpdateTarget()
		{
			Air::Client::PhaseOption opt;
			opt.AddIndex(enPI_Shadow);

			for(U32 i=0;i<m_vecCSMCamera.size();i++){
	
				m_pShadowDepth->SetClearFlag(true,true,true);
				if(m_pShadowDepth->BeforeUpdate()){
					m_vecCSMCamera[i]->Render2D(m_pShadowDepth->GetWidth(),m_pShadowDepth->GetHeight());
					RenderSystem::GetSingleton()->RenderPhase(opt);
					m_pShadowDepth->AfterUpdate();
				}

				//BlurRenderTarget(m_pShadowDepthTemp,m_pShadowDepth);

				//Shadow Mask
				if(i==0){
					m_pShadowMask->SetClearFlag(false,true,false);
				}else{
					m_pShadowMask->SetClearFlag(false,false,false);
				}
				if(m_pShadowMask->BeforeUpdate()){
					Float4	matArray[14];
					Float44*	pMVPInv	=	(Float44*)&matArray[0];
					Float44*	pSVP	=	(Float44*)&matArray[4];
					Float44*	pSVPInv	=	(Float44*)&matArray[8];

					m_pMaskMaterial->GetConstantBuffer()->GetBuffer();
					*pMVPInv	=	m_pMainCamera->GetViewProjMatrix();
					pMVPInv->Inverse();

					*pSVP	=	m_vecCSMCamera[i]->GetViewProjMatrix();
					*pSVPInv	=	*pSVP;
					pSVPInv->Inverse();
					matArray[12]	=	Float4(1.0f/m_pShadowMask->GetWidth(),1.0f/m_pShadowMask->GetHeight(),1.0f/m_pShadowMask->GetWidth(),1.0f/m_pShadowMask->GetHeight());
					matArray[13]	=	m_pMainCamera->GetPosition();
					m_pMaskMaterial->GetConstantBuffer()->UpdateData(matArray);
					m_pMaskMaterial->RenderOneObject(m_pQuad);

					m_pShadowMask->AfterUpdate();
				}
			}
			return true;
		}
		void CascadedShadowMap::BlurRenderTarget( RenderTarget* pDst,RenderTarget* pSrc )
		{
			if(pDst==NULL	||	pSrc	==	NULL)
				return;
			if(pDst->BeforeUpdate()){

				RenderSystem::GetSingleton()->GetDevice()->SetSRV(enPS,0,pSrc->GetSRV());
				m_pBlurX->RenderOneObject(m_pQuad);

				pDst->AfterUpdate();
			}

			if(pSrc->BeforeUpdate()){
				RenderSystem::GetSingleton()->GetDevice()->SetSRV(enPS,0,pDst->GetSRV());
				m_pBlurY->RenderOneObject(m_pQuad);

				pSrc->AfterUpdate();
			}
		}
	}
}