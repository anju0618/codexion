/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakino <amakino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 00:27:04 by amakino           #+#    #+#             */
/*   Updated: 2026/06/22 00:40:02 by amakino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <unistd.h>
# include <stdio.h>
# include <pthread.h>
# include <sys/time.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

/**arg structure**/
typedef struct s_config
{
	int		num_coders;
	int		time_burnout;
	int		time_compile;
	int		time_debug;
	int		time_refactor;
	int		num_compiles_req;
	int		dongle_cooldown;
	char	*scheduler;
}	t_config;

/**dongle structure**/
typedef struct s_dongle
{
	int				id;
	pthread_mutex	mutex;
	pthread_cond_t	cond;
	long long		cooldown_end;
}	t_dongle;

/**coder structure**/
typedef struct s_coder
{
	int				id;
	pthread_cond_t	thread_id;
	int				complile_count;
	longlong		deadline;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	t_config		*config;
}	t_coder;
/** code2_parsing.c **/
int	is_numeric_str(char *str);
int	parse_arguments(t_config *config, char **av);
int	validate_conf(t_config *config, char **av);

#endif