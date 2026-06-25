/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   code5_actions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakino <amakino@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 16:54:24 by amakino           #+#    #+#             */
/*   Updated: 2026/06/25 17:40:43 by amakino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * @brief 左右のドングリをロックする
 * @note 奇数IDのコーダーは[左->右]、偶数IDのコーダーは[右->左]の順で
 * どんぐりを取得することで、円卓全員による同時デッドロック防止
 */
void	acquire_dongles(t_coder *coder)
{
	if (coder->id % 2 != 0)
	{
		pthread_mutex_lock(&coder->left_dongle->mutex);
		print_state(coder, "has taken a dongle");
		pthread_mutex_lock(&coder->right_dongle->mutex);
		print_state(coder, "has taken a dongle");
	}
	else
	{
		pthread_mutex_lock(&coder->right_dongle->mutex);
		print_state(coder, "has taken a dongle");
		pthread_mutex_lock(&coder->left_dongle->mutex);
		print_state(coder, "has taken a dongle");
	}
}

/**
 * @brief 使用し終えた左右のどんぐりのロックを解放する
 * @note 獲得時と逆になるようアンロックを行い、状態の整合性を保ちます。
 */
void	release_dongles(t_coder *coder)
{
	if (coder->id % 2 != 0)
	{
		pthread_mutex_unlock(&coder->right_dongle->mutex);
		pthread_mutex_unlock(&coder->left_dongle->mutex);
	}
	else
	{
		pthread_mutex_unlock(&coder->left_dongle->mutex);
		pthread_mutex_unlock(&coder->right_dongle->mutex);
	}
}
