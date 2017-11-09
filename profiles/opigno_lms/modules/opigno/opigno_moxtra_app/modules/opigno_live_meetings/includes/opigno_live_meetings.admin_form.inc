<?php

function opigno_live_meetings_form_opigno_moxtra_app_moxtra_configuration_form_alter(&$form, &$form_state, $form_id)
{
  $org_id = variable_get('opigno_moxtra_app_org_id');
  if (!empty($org_id)) {

    $info = opigno_moxtra_app_api_opigno_get_max_users_meetings_organization();

    if (!isset($info->max_start_meeting_users)) {
      $form['live_meetings_error'] = array(
        '#type' => 'markup',
        '#markup' => '<p>'. t('Error while catching the max users information') .'</p>'
      );
      return $form;
    }

    if ($info->max_start_meeting_users == 0) {
      $form['live_meetings_info'] = array(
        '#type' => 'markup',
        '#markup' => '<p>'. t('You are using the free version of Live Meetings.') .' '.
          l(t('Upgrade !'), OPIGNO_MOXTRA_APP_OPIGNO_RENEW_GATEWAY. $org_id, array('attributes' => array('target' => '_blank'))) .
          '</p>'
      );
      return $form;
    }

    $form['live_meetings_info'] = array(
      '#type' => 'markup',
      '#markup' => '<p>'.
        t('You have assigned @current / @max users to create/start live meetings.',
          array('@current' => $info->current_start_meeting_users, '@max' => $info->max_start_meeting_users)) .' '.
        l(t('Want more ?'), OPIGNO_MOXTRA_APP_OPIGNO_RENEW_GATEWAY. $org_id, array('attributes' => array('target' => '_blank'))).
        '</p>',
    );



    $form['live_meetings_users_fields'] = array(
      '#type' => 'fieldset',
      '#title' => t('Users who can create/start live meetings')
    );


    $form['live_meetings_users_fields']['user_add_container'] = array(
      '#prefix' => '<div class="container-inline">',
      '#suffix' => '</div>',
    );

    $form['live_meetings_users_fields']['user_add_container']['admin_users_add'] = array(
      '#type' => 'textfield',
      '#autocomplete_path' => 'user/autocomplete'
    );

    $form['live_meetings_users_fields']['user_add_container']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Add a user'),
      '#name' => 'add_user'
    );


    $query = db_select('opigno_live_meetings_users_can_start_meetings', 't')
      ->fields('t')
      ->execute();



    while($row = $query->fetchAssoc()) {
      $user = user_load($row['user_id']);
      $container_name = 'user_'. $user->uid;


      $form['live_meetings_users_fields'][$container_name] = array(
        '#type' => 'container',
        '#attributes' => array(
          'class' => array('container-inline')
        )
      );

      $form['live_meetings_users_fields'][$container_name]['user_'. $user->uid .'_submit'] = array(
        '#type' => 'submit',
        '#value' => t('Revoke'),
        '#name' => 'revoke_'. $user->uid
      );

      $form['live_meetings_users_fields'][$container_name]['user_'. $user->uid .'_label'] = array(
        '#type' => 'markup',
        '#markup' => ' '. $user->name
      );

    }


    $form['#validate'][] = 'opigno_live_meetings_form_opigno_moxtra_app_moxtra_configuration_form_validate';
  }

  return $form;
}


function opigno_live_meetings_form_opigno_moxtra_app_moxtra_configuration_form_validate(&$form, &$form_state)
{
  if($form_state['triggering_element']['#name'] == 'add_user' && !empty($form_state['values']['admin_users_add']))
  {
    $user_to_add = user_load_by_name($form_state['values']['admin_users_add']);
    if ($user_to_add == false) {
      form_set_error('', t('The user does not exists'));
      return;
    }

    $exists = db_query('SELECT 1 FROM opigno_live_meetings_users_can_start_meetings WHERE user_id = :uid',
      array(':uid' => $user_to_add->uid))
      ->fetchField();

    if ($exists) {
      form_set_error('', t('This user can already create/start live meetings'));
      return;
    }

    $response = opigno_moxtra_app_api_opigno_allow_user_meetings(array(
      'uid' => $user_to_add->uid,
      'name' => $user_to_add->name,
      'timezone' => $user_to_add->timezone
    ));

    if (isset($response->opigno_error_message)) {
      form_set_error('', $response->opigno_error_message);
      return;
    }

    db_insert('opigno_live_meetings_users_can_start_meetings')
      ->fields(array('user_id' => $user_to_add->uid))->execute();
  }


  else if (strpos($form_state['triggering_element']['#name'], 'revoke_') === 0)
  {
    $user_to_remove = user_load(substr($form_state['triggering_element']['#name'], 7));

    $response = opigno_moxtra_app_api_opigno_deny_user_meetings(array(
      'uid' => $user_to_remove->uid,
      'name' => $user_to_remove->name,
      'timezone' => $user_to_remove->timezone
    ));

    if (isset($response->opigno_error_message)) {
      form_set_error('', $response->opigno_error_message);
      return;
    }

    db_delete('opigno_live_meetings_users_can_start_meetings')
      ->condition('user_id', $user_to_remove->uid, '=')->execute();
  }
}
