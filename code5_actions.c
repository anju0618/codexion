/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   code5_actions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakino <amakino@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 16:54:24 by amakino           #+#    #+#             */
/*   Updated: 2026/06/25 16:54:26 by amakino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
