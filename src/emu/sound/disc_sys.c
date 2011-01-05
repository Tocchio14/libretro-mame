/************************************************************************
 *
 *  MAME - Discrete sound system emulation library
 *
 *  Written by Keith Wilkins (mame@esplexo.co.uk)
 *
 *  (c) K.Wilkins 2000
 *  (c) D.Renaud 2003-2004
 *
 ************************************************************************
 *
 * DSO_OUTPUT            - Output node
 * DSO_TASK              - Task node
 *
 * Task and list routines
 *
 ************************************************************************/

struct dso_csvlog_context
{
	FILE *csv_file;
	INT64 sample_num;
	char name[32];
};

struct dso_wavlog_context
{
	wav_file *wavfile;
	char name[32];
};


/*************************************
 *
 *  Task node (main task execution)
 *
 *************************************/

static void task_check(discrete_task *task, discrete_task *dest_task)
{
	int inputnum;

	/* Determine, which nodes in the task are referenced by nodes in dest_task
     * and add them to the list of nodes to be buffered for further processing
     */
	for_each(node_description *, node_entry, &task->list)
	{
		node_description *task_node = node_entry.item();

		for_each(node_description *, step_entry, &dest_task->list)
		{
			node_description *dest_node = step_entry.item();

			/* loop over all active inputs */
			for (inputnum = 0; inputnum < dest_node->active_inputs(); inputnum++)
			{
				int inputnode = dest_node->input_node(inputnum);
				if IS_VALUE_A_NODE(inputnode)
				{
					if (NODE_DEFAULT_NODE(task_node->block_node()) == NODE_DEFAULT_NODE(inputnode))
					{
						discrete_source_node *source;
						int i, found = -1;

						for (i = 0; i < task->numbuffered; i++)
							if (task->nodes[i]->block_node() == inputnode)
							{
								found = i;
								break;
							}

						if (found<0)
						{
							if (task->numbuffered >= DISCRETE_MAX_TASK_OUTPUTS)
								fatalerror("dso_task_start - Number of maximum buffered nodes exceeded");

							task->node_buf[task->numbuffered] = auto_alloc_array(task_node->device->machine, double,
									((task_node->sample_rate() + STREAMS_UPDATE_FREQUENCY) / STREAMS_UPDATE_FREQUENCY));
							task->source[task->numbuffered] = (double *) dest_node->input[inputnum];
							task->nodes[task->numbuffered] = task_node->device->discrete_find_node(inputnode);
							i = task->numbuffered;
							task->numbuffered++;
						}
						task_node->device->discrete_log("dso_task_start - buffering %d(%d) in task %p group %d referenced by %d group %d", NODE_INDEX(inputnode), NODE_CHILD_NODE_NUM(inputnode), task, task->task_group, dest_node->index(), dest_task->task_group);

						/* register into source list */
						source = auto_alloc(dest_node->device->machine, discrete_source_node);
						dest_task->source_list.add_tail(source);
						source->task = task;
						source->output_node = i;

						/* point the input to a buffered location */
						dest_node->input[inputnum] = &source->buffer;

					}
				}
			}
		}
	}
}

#define DISCRETE_DECLARE_TASK	discrete_task *task = (discrete_task *) node->context;

DISCRETE_START( dso_task_start )
{
	DISCRETE_DECLARE_TASK

	task->task_group = (int) DISCRETE_INPUT(0);

	if (task->task_group < 0 || task->task_group >= DISCRETE_MAX_TASK_GROUPS)
		fatalerror("discrete_dso_task: illegal task_group %d", task->task_group);

	for_each(discrete_task *, dest_task, &node->device->task_list)
	{
		if (task->task_group > dest_task.item()->task_group)
			task_check(dest_task.item(), task);
	}

}

DISCRETE_STEP( dso_task_end )
{
	DISCRETE_DECLARE_TASK

	int i;

	for (i = 0; i < task->numbuffered; i++)
		*(task->ptr[i]++) = *task->source[i];
}

DISCRETE_STEP( dso_task_start )
{
	DISCRETE_DECLARE_TASK

	/* update source node buffer */
	for_each(discrete_source_node *, sn, &task->source_list)
	{
		sn.item()->buffer = *sn.item()->ptr++;
	}
}


DISCRETE_RESET( dso_task )
{
	/* nothing to do - just avoid being stepped */
}

DISCRETE_STEP( dso_output )
{
	stream_sample_t **output = (stream_sample_t **) &node->context;
	double val;

	/* Add gain to the output and put into the buffers */
	/* Clipping will be handled by the main sound system */
	val = DISCRETE_INPUT(0) * DISCRETE_INPUT(1);
	**output = val;
	(*output)++;
}

DISCRETE_RESET( dso_output )
{
	/* nothing to do - just avoid being stepped */
}

DISCRETE_START( dso_csvlog )
{
	DISCRETE_DECLARE_CONTEXT(dso_csvlog)

	int log_num, node_num;

	log_num = node->device->same_module_index(*node);
	context->sample_num = 0;

	sprintf(context->name, "discrete_%s_%d.csv", node->device->tag(), log_num);
	context->csv_file = fopen(context->name, "w");
	/* Output some header info */
	fprintf(context->csv_file, "\"MAME Discrete System Node Log\"\n");
	fprintf(context->csv_file, "\"Log Version\", 1.0\n");
	fprintf(context->csv_file, "\"Sample Rate\", %d\n", node->sample_rate());
	fprintf(context->csv_file, "\n");
	fprintf(context->csv_file, "\"Sample\"");
	for (node_num = 0; node_num < node->active_inputs(); node_num++)
	{
		fprintf(context->csv_file, ", \"NODE_%2d\"", NODE_INDEX(node->input_node(node_num)));
	}
	fprintf(context->csv_file, "\n");
}

DISCRETE_STOP( dso_csvlog )
{
	DISCRETE_DECLARE_CONTEXT(dso_csvlog)

	/* close any csv files */
	if (context->csv_file)
		fclose(context->csv_file);
}

DISCRETE_STEP( dso_csvlog )
{
	DISCRETE_DECLARE_CONTEXT(dso_csvlog)

	int nodenum;

	/* Dump any csv logs */
	fprintf(context->csv_file, "%" I64FMT "d", ++context->sample_num);
	for (nodenum = 0; nodenum < node->active_inputs(); nodenum++)
	{
		fprintf(context->csv_file, ", %f", *node->input[nodenum]);
	}
	fprintf(context->csv_file, "\n");
}

DISCRETE_START( dso_wavlog )
{
	DISCRETE_DECLARE_CONTEXT(dso_wavlog)

	int log_num;

	log_num = node->device->same_module_index(*node);
	sprintf(context->name, "discrete_%s_%d.wav", node->device->tag(), log_num);
	context->wavfile = wav_open(context->name, node->sample_rate(), node->active_inputs()/2);
}

DISCRETE_STOP( dso_wavlog )
{
	DISCRETE_DECLARE_CONTEXT(dso_wavlog)

	/* close any wave files */
	if (context->wavfile)
		wav_close(context->wavfile);
}

DISCRETE_STEP( dso_wavlog )
{
	DISCRETE_DECLARE_CONTEXT(dso_wavlog)

	double val;
	INT16 wave_data_l, wave_data_r;

	/* Dump any wave logs */
	/* get nodes to be logged and apply gain, then clip to 16 bit */
	val = DISCRETE_INPUT(0) * DISCRETE_INPUT(1);
	val = (val < -32768) ? -32768 : (val > 32767) ? 32767 : val;
	wave_data_l = (INT16)val;
	if (node->active_inputs() == 2)
	{
		/* DISCRETE_WAVLOG1 */
		wav_add_data_16(context->wavfile, &wave_data_l, 1);
	}
	else
	{
		/* DISCRETE_WAVLOG2 */
		val = DISCRETE_INPUT(2) * DISCRETE_INPUT(3);
		val = (val < -32768) ? -32768 : (val > 32767) ? 32767 : val;
		wave_data_r = (INT16)val;

		wav_add_data_16lr(context->wavfile, &wave_data_l, &wave_data_r, 1);
	}
}
