﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "Command/UnfocusedCommand.h"
#include "UEPrototype.h"
#include "EditorModulesManager.h"
#include "Command/CommandConstraintManager.h"
#include "Command/CmdFocusedConstraint.h"
#include "Command/CmdHighlightedConstraint.h"
#include "Command/CmdUnfocusedConstraint.h"
#include "ActorInfo/Outliner.h"
#include "ActorInfo/ActorConstraintMarker.h"

UOutliner* UUnfocusedCommand::Outliner = nullptr;
UActorConstraintMarker* UUnfocusedCommand::ActorConstraintMarker = nullptr;

UUnfocusedCommand::UUnfocusedCommand()
{
	VP_CTOR;

	if (!IsValid(UCommandConstraintManager::GetGlobalCommandConstraintManager())) return;

	if (!IsValid(UCommandConstraintManager::GetGlobalCommandConstraintManager()->GetCmdFocusedConstraint()))return;

	if (!IsValid(UCommandConstraintManager::GetGlobalCommandConstraintManager()->GetCmdHighlightedConstraint()))return;

	UCmdFocusedConstraint * FocusedConstraint = UCommandConstraintManager::GetGlobalCommandConstraintManager()->GetCmdFocusedConstraint();

	if (IsValid(FocusedConstraint) == false)
	{
		VP_LOG(Warning, TEXT("%s가 유효하지 않습니다."), *UCommandConstraintManager::GetGlobalCommandConstraintManager()->GetCmdFocusedConstraint()->GetName());
		return;
	}

	UCmdHighlightedConstraint* HighlightedConstraint = UCommandConstraintManager::GetGlobalCommandConstraintManager()->GetCmdHighlightedConstraint();

	if (IsValid(HighlightedConstraint) == false)
	{
		VP_LOG(Warning, TEXT("%s가 유효하지 않습니다."), *UCommandConstraintManager::GetGlobalCommandConstraintManager()->GetCmdHighlightedConstraint()->GetName());
		return;
	}

	IActorCmdConstraint* ConstraintInterface = Cast<IActorCmdConstraint>(FocusedConstraint);
	if (ConstraintInterface == nullptr)
	{
		VP_LOG(Warning, TEXT(";;;"));
		return;
	}
	Constraints.Add(FocusedConstraint);
	Constraints.Add(HighlightedConstraint);

	if (Outliner != nullptr && Outliner->IsValidLowLevel() && ActorConstraintMarker != nullptr &&
		ActorConstraintMarker->IsValidLowLevel())
	{
		VP_LOG(Log, TEXT("UnFocusedCommand의 멤버가 유효하다네요?"));
		return;
	}

	/* 초기화하는 데 필요한 객체를 가지고 있는 모듈의 유효성을 검사합니다 */
	UEditorModulesManager* EditorModulesManager =
		UEditorModulesManager::GetGlobalEditorModulesManager();
	if (IsValid(EditorModulesManager) == false)
	{
		VP_LOG(Warning, TEXT("%s가 유효하지 않습니다"), *UEditorModulesManager::StaticClass()->GetName());
		return;
	}

	UCommandConstraintManager* CommandConstraintManager = UCommandConstraintManager::GetGlobalCommandConstraintManager();
	if (IsValid(CommandConstraintManager) == false)
	{
		VP_LOG(Warning, TEXT("%s가 유효하지 않습니다."), *UCommandConstraintManager::GetGlobalCommandConstraintManager()->GetName());
		return;
	}
	


	/* 초기화를 수행합니다 */
	VP_LOG(Warning, TEXT("[DEBUG] 에디터 모듈이 초기화가 되어있습니다."));
	Outliner = UOutliner::GetGlobalOutliner();
	ActorConstraintMarker = UActorConstraintMarker::GetGlobalActorConstraintMarker();




	/* 초기화된 객체들에 대한 유효성 검사를 실행합니다 */
	if (IsValid(Outliner) == false)
	{
		VP_LOG(Warning, TEXT("%s가 유효하지 않습니다"), *UOutliner::StaticClass()->GetName());
		return;
	}
	if (IsValid(ActorConstraintMarker) == false)
	{
		VP_LOG(Warning, TEXT("%s가 유효하지 않습니다"), *UActorConstraintMarker::StaticClass()->GetName());
		return;
	}
	if (Constraints.Num() == 0)
	{
		VP_LOG(Warning, TEXT("제약 조건이 올바르게 설정되지 않았습니다."));
		return;
	}

	// DEBUG
	VP_LOG(Warning, TEXT("[DEBUG] %s : %d, Pointer Address : %x"), *Outliner->GetName(), Outliner->GetUniqueID(), &Outliner);
	VP_LOG(Warning, TEXT("[DEBUG] %s : %d, Pointer Address : %x"), *ActorConstraintMarker->GetName(), ActorConstraintMarker->GetUniqueID(), &ActorConstraintMarker);

}

void UUnfocusedCommand::ExecuteIf()
{
	/* 실행 전 유효성을 검사합니다 */
	if (IsValid(Outliner) == false)
	{
		VP_LOG(Warning, TEXT("명령을 실행하는데 %s가 유효하지 않습니다."), *UOutliner::StaticClass()->GetName());
		return;
	}
	if (IsValid(ActorConstraintMarker) == false)
	{
		VP_LOG(Warning, TEXT("명령을 실행하는데 %s가 유효하지 않습니다."), *UActorConstraintMarker::StaticClass()->GetName());
		return;
	}

	
	/* 명령이 제약 조건을 만족하는 지 확인합니다 */
	
	for (const auto& it : Constraints)
	{
		if (it->CheckConstraint(Target) == false)
		{
			ActorConstraintMarker->MarkActor(Target.Target, EActorConstraintState::CSTR_Unfocused);
			Outliner->EraseActorOutline();
			return;
		}
	}
	
}

void UUnfocusedCommand::InitActorCommand(FActorConstraintInfo TargetInfo)
{
	Target = TargetInfo;
}
